# ClothesFS

Simple, but yet powerful file system.


## Header

In beginning of system.
Contains magic and other info.

    JUMP1       8 bytes   Jump instruction 1
    JUMP2       8 bytes   Jump instruction 2
    JUMP3       8 bytes   Jump instruction 3
    JUMP4       8 bytes   Jump instruction 4
    ID          4 bytes   0x00420041
    blocksize   2 bytes   Default 512 (match sector size)
    flags       1 byte    0x00 == No flags
                          0x01 == Support mirror
                          0x02 == Encrypted
                          0x04 == Logical volume group
                          0x08 == Merge volume group
    grpindex    1 byte    Index in volume group (if enabled)
    volid       8 bytes   Volume id
    size        8 bytes   Volume size
    name       32 bytes   Volume name
    groupid     8 bytes   Volume group id (if enabled)
    root1       4 bytes   Pointer to root metadata
    used        4 bytes   Used blocks
    journal1    4 bytes   Pointer to first journal chain
    journal2    4 bytes   Pointer to second journal chain
    freechain   4 bytes   Pointer to free block chain
    ...
    ... If blocksize > 512, copies at 513, 1025, 1537, 2049 (on first block only)
    ID          4 bytes   0x00422400
    root2       4 bytes   Pointer to root metadata, should be same as root1
    journal1    4 bytes   Pointer to first journal chain
    journal2    4 bytes   Pointer to second journal chain


For example block size 512 means we can have
2^32 * 512 blocks = 2 TiB

Bigger disk needs bigger block.
For example 1024 byte blocks doubles maximum size to 4 TiB.


## Volume groups

ClothesFS support two different volume groups.

First one is logical volume group, where disk space can be extended
by appending new disk to array.
Order of volumes is defined by grpindex, so that smaller comes earlier.
Maximum number of volumes is 256.
Whole volume group is handled as one big volume, so block size needs to be
big enough to fit WHOLE volume array in index.
It's recommended that logical groups choose at least 1k or 2k block size.
For future proof installations, 4k block size is recommended.
In logical view block numbers continue from where previous volume left.
Let's say first volume is 5 GB, and block size is 1k there is 5242880 blocks in it.
So first block in next volume will get index 5242881.

Another way to form a volume group is to make merged FS.
This means effectively onion layer. Let's assume you have 1 GiB disk,
and it's running out soon. You want to keep files but easily add another disk on it.
You can enable merge volume group at any time and add another disk to the group.
There's a catch. All the data in previous layer is read only,
and all write actions are performed to the new layer. This is effectively COW.
However WHOLE file needs to be copied at one when first time written,
and can cause unwanted delay. One file is written to new layer,
it is not accessed from old one any more.
Access to files are done from newest to oldest.
If file is found from layer, it can be read from that layer completely.
This way different volumes can maintain their block ids without any sync.


## Data

Starts after header.
Three kind of data: metadata and payload.


### Metadata

Whole filesystem tree starts from root.
Pointer to this entry is given in header. It consist all root level objects.
Objects are just 32 bit unsigned integers after each other,
pointing to block of metadata.


Metadata may contain file or folder.

    ID       2 bytes  0x4200
    type     1 byte   0x00 = Invalid/Free
                      0x02 = File
                      0x04 = Directory
                      0x08 = File continued
                      0x10 = Directory continued
                      0x80 = Journal
    attrib   1 bytes  File attributes
                      0x00 = No attributes
                      0x01 = Execute
                      0x02 = Read
                      0x04 = Write
                      0x08 = File is link
                      0x10 = Other execute
                      0x20 = Other read
                      0x40 = Other write
                      0x80 = File is special

In case of 0x02 or 0x04:

    size     8 bytes  Size of file or directory
    namelen  4 bytes  Length of name (little endian)
    name     X bytes  Name of file or folder (namelen bytes, may continue in another block)
    padding  X bytes  To 4 byte boundary

In case of 0x02 or 0x08 these are payload blocks.
In case of 0x04 or 0x10 these are index entries.

    entry1   4 bytes  Pointer to block 1
    entry2   4 bytes  Pointer to block 2
    ...
    entryN   4 bytes  Pointer to block N

If value is zero (0x00000000), then it's empty block.
It also means there's no more entries available.


Last entry in block has special meaning.
If it's value is zero, there's no extra data.
If it has non zero value, it's number of block where metadata continues.


### Journals

Journal is special block which contains temporary journaling data.
It starts with:

    0x42428080

Rest of the block is operating system specific journaling data,
which will be written to proper places later on.

Last 4 bytes can be zero, or number of another block where journal continues.


### Payload

Payload is almost pure data. Total length of data is defined in metadata entry.
After that data is read from defined payload blocks until everything is read.

Payload block has following header:

    ID       2 bytes  0x4242
    type     1 byte   0x00 = Free/Invalid
                      0x01 = Used
                      0x02 = Freed
    algo     1 byte   0x00 = Disabled
                      0x01 = XOR
                      0x02 = CRC32
                      0x04 = sum_of_bytes mod 2^32
                      0x08 = ??
    check    4 byte   Checksum of whole block (without header)
                      Does not exists if algo is disabled
    data     block_size - 4/8 bytes


Checksum is calculated with defined algorithms.
There can be multiple different algorithms involved.
In that case they are applied from lowest bit to highest.
For example 0x06 means first 0x02 and then 0x04.
Combining results are then done with XOR operator.


## Formatting

When formatting the volume all metadata needs to be put in place.
By default free blocks are marked as invalid metadata:

    0x42000000

They are chained together, so last 4 bytes of every block is pointer to next one.
Some blocks can be marked as journal.
Header is filled accordingly with gathered and written data.


## Free block handling

When filesystem is in use, free blocks are got from freechain.
First entry from chain is taken, and it's next block is put as new beginning of chain.
Similarly when freeing a block, it is put as first one.
