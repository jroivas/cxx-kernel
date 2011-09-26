#include "mm.h"
#include "paging.h"

static MM __mm_static_instance;
static bool initted = false;
static unsigned int __mm_mutex;
static unsigned int __mm_free_mutex;
static unsigned int *__mm_last_free = 0;

inline void *operator new(size_t, void *p) throw() { return p; }

struct PtrInfo
{
	unsigned int *prev;
	unsigned int *next;
	unsigned int size;
};

#if 0
/* Free info
 * Marking up freed pages to a binary tree.
 * Initially this is empty (NULL).
 * Once a page is freed from use it's added to this tree.
 * If this tree is empty, it's added as a root item,
 * if there's item(s) on this tree sizes are compared and 
 *  if new item size if smaller than node, it's put on left side.
 *  if new item size is larger or equl to node, it's put on right side.
 */
class FreeInfo
{
public:
#if 0
	enum AllocInfoType {
		AllocInfoNone = 0,
		AllocInfoRoot,
		AllocInfoMap,
		AllocInfoFree,
		AllocInfoUsed
	};
#endif

	FreeInfo();

	//void allocChunk(unsigned int cnt);
	//void assignChunk(void *chunk, unsigned int cnt);
	inline unsigned int size() { return m_size; }
	inline void *ptr() { return m_ptr; }
	void assignTo(void *ptr, unsigned int size);
	void append(FreeInfo *f);
#if 0
	void *chunk();

	void linkTo(AllocInfo *info);
	void addChild(AllocInfo *child);
	void setType(AllocInfoType type);
#endif

private:
	FreeInfo *leftChild;
	FreeInfo *rightChild;

	unsigned int m_size;
	void *m_ptr;
#if 0
	AllocInfo *prev;
	AllocInfo *next;
	unsigned int m_size;
	void *m_chunk;
	//AllocInfoType m_type;
	unsigned int m_size;
	void *m_ptr;
#endif
};

/* Alloc info
 * Marking up allocs in one (4k) page.
 * Once this info page is full, one need to reserve another page.
 * Purpose is to mark allocations as:
 * ---
 * FLAGS OF A
 * SIZE OF A
 * PTR TO A
 * ---
 * FLAGS OF B
 * SIZE OF B
 * PTR TO B
 */
class AllocInfo
{
public:
	AllocInfo();
	AllocInfo(void *from);

	bool assign(void *ptr);
	bool append(void *ptr);
	bool isValid() { return (m_chunk!=NULL); }
	bool isFull();
	bool addAlloc(void *ptr, unsigned int size);
	FreeInfo *freeAlloc(void *ptr);

private:
	enum AllocFlags {
		AllocFlagNone = 0,
		AllocFlagUsed = 0x01,
		AllocFlagMapped = 0x02,
		AllocFlagCheck = 0x80
	};
	struct Data {
		unsigned int flags;
		unsigned int size;
		void* ptr;
	};
	void *m_chunk;
	void *m_freebase;
};

// Need to be here since now AllocInfo class is defined
static AllocInfo __mm_allocinfo_initial;


AllocInfo::AllocInfo()
{
	m_chunk = NULL;
	m_freebase = NULL;
}

AllocInfo::AllocInfo(void *from)
{
	m_chunk = from;
}

bool AllocInfo::isFull()
{
	if (m_chunk==NULL || m_freebase==NULL) return false;

	char *a = (char*)m_chunk;
	char *b = (char*)m_freebase;

	// We have only ONE page reserved, if freebase is full or over it, we're full
	//if (b+sizeof(void*)+sizeof(unsigned int)<a+PAGE_SIZE) return false;
	if (b+sizeof(Data)<a+PAGE_SIZE) {
		unsigned int *p = (unsigned int*)m_chunk;
		if (*p!=0) {
		}
		return false;
	}

	return true;
}

bool AllocInfo::assign(void *ptr)
{
	if (m_chunk!=NULL) return false;
	m_chunk = ptr;
	//m_freebase = ptr;
	*(unsigned int*)m_chunk = 0;
	m_freebase = (unsigned char*)ptr + sizeof(unsigned int *);

	return true;
}

bool AllocInfo::append(void *ptr)
{
	if (m_chunk!=NULL) return false;

	AllocInfo *tmp = new (ptr)AllocInfo;
	//*(unsigned int*)m_chunk = (unsigned int)ptr;
	*(unsigned int*)m_chunk = (unsigned int)tmp;

	return true;
}

FreeInfo *AllocInfo::freeAlloc(void *ptr)
{
	if (ptr>=m_chunk && ptr<=m_freebase) {
		void *p = (void*)((unsigned int)ptr-sizeof(Data)+sizeof(void*));
		Data *tmp = new (p)Data;
		tmp->flags &= ~AllocFlagUsed;

		FreeInfo *fi = new FreeInfo();
		fi->assignTo(ptr, tmp->size);

		return fi;
	} else {
		return NULL;
	}
}

bool AllocInfo::addAlloc(void *ptr, unsigned int size)
{
	if (isFull()) return false;

	// Adding alloc to the chunk
	Data *tmp = new (&m_freebase)Data;
	tmp->flags = AllocFlagCheck | AllocFlagMapped | AllocFlagUsed;
	tmp->size = size;
	tmp->ptr = ptr;
	m_freebase = (unsigned char*)m_freebase + sizeof(Data);
#if 0
	ptr32_t p = (ptr32_t)m_freebase;
	*p = size;
	p++;
	*p = (ptr32_val_t)ptr;
	p++;

	m_freebase = (void*)p;
#endif

	return true;
}

FreeInfo::FreeInfo()
{
	leftChild = NULL;
	rightChild = NULL;
	m_ptr = NULL;
	m_size = 0;
}

void FreeInfo::assignTo(void *ptr, unsigned int size)
{
	m_ptr = ptr;
	m_size = size;
}

void FreeInfo::append(FreeInfo *f)
{
	if (f==NULL) return;

	FreeInfo *tmp = this;
	if (tmp->size() < m_size) {
		if (leftChild!=NULL) leftChild->append(f);
		else leftChild = f;
	} else {
		if (rightChild!=NULL) rightChild->append(f);
		else rightChild = f;
	}
}

//XXX DEPRECATED static AllocInfo __mm_alloc_root;

#if 0
AllocInfo::AllocInfo()
{
	m_size = 0;
	m_ptr = NULL;
	prev = NULL;
	next = NULL;
	m_chunk = NULL;
	m_cnt = 0;
	//m_type = AllocInfoNone;
}

void AllocInfo::assignTo(void *ptr, unsigned int size)
{
	m_ptr = ptr;
	m_size = size;
}
#endif


#if 0
void AllocInfo::allocChunk(unsigned int cnt)
{
	m_chunk = MM::instance()->alloc(cnt);
	if (m_chunk!=NULL) {
		m_cnt = cnt;
	}
}

void AllocInfo::assignChunk(void *chunk, unsigned int cnt)
{
	m_chunk = chunk;
	m_cnt = cnt;
}
#endif

#if 0
void AllocInfo::setType(AllocInfoType type)
{
	(void)type;
	//m_type = type;
}

unsigned int AllocInfo::size()
{
	return m_cnt*PAGE_SIZE;
}

void AllocInfo::linkTo(AllocInfo *info)
{
	if (info==NULL) return;

	AllocInfo *tmp = next;
	if (tmp!=NULL) {
		while (tmp!=NULL) {
			tmp = next->next;
		}
	}
	next=info;

	if (info->prev==NULL) {
		info->prev = this;
	} else {
		while (info->next!=NULL) {
			info = info->next;
		}
		info->next = this;
	}
}
#endif
#endif

MM::MM()
{
	m = Mutex(&__mm_mutex);
	mf = Mutex(&__mm_free_mutex);
	//m_freed = NULL;
}

MM *MM::instance()
{
#if 0
	if (!initted) {
		//__mm_static_instance = MM();
		//__mm_static_instance.root = &__mm_alloc_root;
		__mm_static_instance.m_root = new (&__mm_allocinfo_initial)AllocInfo;
		//__mm_static_instance.m_freed = new (&__mm_freedinfo_initial)FreedInfo;
		initted = true;
	}
#endif
	return &__mm_static_instance;
}

/* Just a simple wrapper */
void *MM::allocPage(size_t cnt)
{
	Paging p;
	return p.alloc(cnt);
}

void *MM::findAvail(size_t size)
{
	PtrInfo *tmp = (PtrInfo*)(__mm_last_free-sizeof(PtrInfo));
	while (tmp!=NULL) {
		if (tmp->size == size) {
			PtrInfo *prev = (PtrInfo*)(tmp->prev-sizeof(PtrInfo));
			PtrInfo *next = (PtrInfo*)(tmp->next-sizeof(PtrInfo));

			prev->next = (unsigned int*)next+sizeof(PtrInfo);
			next->prev = (unsigned int*)prev+sizeof(PtrInfo);
			return (void*)(tmp+sizeof(PtrInfo));
		}
		if (tmp->prev == NULL) tmp = NULL;
		else tmp = (PtrInfo*)(tmp->prev-sizeof(PtrInfo));
	}

	return NULL;
}

void *MM::allocMem(size_t size, AllocType t)
{
	unsigned int m = 0;

	//size+=sizeof(void*)*2+sizeof(unsigned int);
	//unsigned int size2 = size+sizeof(void*)*3;
	unsigned int size2 = size+sizeof(PtrInfo);

	//if (size%PAGE_SIZE>0) m=1;
	//unsigned int cnt = size/PAGE_SIZE+m;

	if (size2%PAGE_SIZE>0) m=1;

	unsigned int cnt = size2/PAGE_SIZE+m;


	unsigned int *ptr = NULL;
	if (t==AllocNormal) {
		void *tmp = findAvail(size);
		if (tmp!=NULL) ptr=(unsigned int*)tmp;
	}

	if (ptr==NULL) ptr = (unsigned int*)allocPage(cnt);

	PtrInfo *tmp = (PtrInfo*)ptr;
	tmp->prev = NULL;
	tmp->next = NULL;
	tmp->size = size;

	ptr += sizeof(PtrInfo);

#if 0
	unsigned int *ptr = (unsigned int*)allocPage(cnt);
	*ptr = 0;
	ptr++;
	*ptr = 0;
	ptr++;
	*ptr = size;
	ptr++;
#endif
	//return allocPage(cnt);
	return ptr;
}

void *MM::allocMemClear(size_t size)
{
	unsigned char *ptr = (unsigned char*)allocMem(size, AllocNormal);
	unsigned char *tmp = ptr;

	if (ptr==NULL) return NULL;

	// This always ensures that the memory is cleared
	while (tmp<ptr+size) {
		*tmp = 0;
		tmp++;
	}

	return (void*)ptr;
}

void *MM::alloc(size_t size, AllocType t)
{
	m.lock();
	void *res = NULL;

	/* First we make sure we got the map. */
#if 0
	if (!m_root->isValid) {
		res = allocPage(1);
		if (res==NULL) return NULL;
		m_root->assign(res);

#if 0
		res = allocPage(1);
		if (res==NULL) return NULL;
		m_freed->assign(res);
#endif
		res = NULL;
	}

	if (m_root->isFull()) {
		res = allocPage(1);
		if (res==NULL) return NULL;
		m_root->append(res);
	}
#endif

	switch (t) {
		case AllocNormal:
			res = allocMem(size, t);
			break;
		case AllocFast:
			res = allocMem(size, t);
			break;
		case AllocClear:
			res = allocMemClear(size);
			break;
		case AllocPage:
			res = allocPage(size);
			break;
		default:
			break;
	}
	m.unlock();
	return res;
}

bool MM::free(void *p)
{
	PtrInfo *tmp = (PtrInfo*)((unsigned int*)p-sizeof(PtrInfo));

	tmp->prev = __mm_last_free;
	tmp->next = NULL;
	if (__mm_last_free != NULL) {
		PtrInfo *last = (PtrInfo*)(__mm_last_free-sizeof(PtrInfo));
		last->next = (unsigned int*)p;
	}


#if 0
	unsigned int *ptr = (unsigned int*)p;
	ptr--;
	*ptr = (unsigned int)__mm_last_free;

	if (__mm_last_free != NULL) {
		unsigned int *last = __mm_last_free;
		last--;
		last--;
		*last = (unsigned int)p;
	}
#endif

	__mm_last_free = (unsigned int*)p;
#if 0
	(void)p;
	if (p==NULL) return false;

	mf.lock();
 
	FreeInfo *tmp = m_root->freeAlloc(p);

	m.lock();

	if (m_freed == NULL) {
		m_freed = tmp;
	} else {
		m_freed->append(tmp);
	}
	m.unlock();

	mf.unlock();
#endif
	return true;
}
