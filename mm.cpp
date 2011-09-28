#include "mm.h"

#ifdef USE_LINUX
#include <sys/mman.h>
#include <stdio.h>
#else
#include "paging.h"
#endif
       
static MM __mm_static_instance;
static volatile ptr_val_t  __mm_mutex;
static ptr_t __mm_last_free = NULL;

struct PtrInfo
{
	ptr_t prev;
	ptr_t next;
	ptr_val_t size;
	ptr_val_t ressize;
};

MM::MM()
{
	m = Mutex(&__mm_mutex);
	m_lastPage = NULL;
	m_freeTop = NULL;
}

MM *MM::instance()
{
	return &__mm_static_instance;
}

/* Just a simple wrapper */
void *MM::allocPage(size_t cnt)
{
#ifdef USE_LINUX
	void *block;
	size_t block_size = cnt * PAGE_SIZE;
	block = mmap((void *)0, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block==(void*)-1) return NULL;

	return block;
#else
	Paging p;
	return p.alloc(cnt);
#endif
}

void *MM::findAvail(size_t size)
{
	if (__mm_last_free==NULL) return NULL;

	PtrInfo *tmp = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));

	//TODO: Merging blocks would be a pop
	//TODO: Splitting blocks would be great as well

	/* Check for existing memory area */
	while (tmp!=NULL) {
		if (tmp->size == size /*|| tmp->ressize > size+sizeof(PtrInfo)*/) {
			PtrInfo *prev = NULL;
			PtrInfo *next = NULL;

			ptr_t lp = (ptr_t)(((ptr_val_t)tmp)+sizeof(PtrInfo));

			if (tmp->prev!=NULL) {
				prev = (PtrInfo*)(((ptr_val_t)tmp->prev)-sizeof(PtrInfo));
			}
			if (tmp->next!=NULL) {
				next = (PtrInfo*)(((ptr_val_t)tmp->next)-sizeof(PtrInfo));
			}

			if (next!=NULL && prev!=NULL) {
				next->prev = tmp->prev;
				prev->next = tmp->next;
			} else if (next!=NULL && prev==NULL){
				next->prev = NULL;
			} else if (next==NULL && prev!=NULL){
				prev->next = NULL;
			} else {
				__mm_last_free = NULL;
			}
	
			if (lp==__mm_last_free) {
				if (tmp->next!=NULL && tmp->next!=lp) __mm_last_free = tmp->next;
				else if (tmp->prev!=NULL && tmp->prev!=lp) __mm_last_free = tmp->prev;
				else __mm_last_free = NULL;
			}

			tmp->next = NULL;
			tmp->prev = NULL;

			return (void*)tmp;
		}
		if (tmp->prev == NULL) tmp = NULL;
		else {
			tmp = (PtrInfo*)(((ptr_val_t)tmp->prev)-sizeof(PtrInfo));
		}
	}

	return NULL;
}

void *MM::allocMem(size_t size, AllocType t)
{
	ptr_val_t m = 0;

	ptr_val_t size2 = size+sizeof(PtrInfo);

	/* Round up to 2^c boundaries */
	ptr_val_t s = size2;
        size_t c=0;

        while (s>0) {
                c++;
                s >>= 1;
        }
        s = 1;
        while (c>0) {
                s <<= 1;
                c--;
        }
        if (size2%(s>>1)==0) s>>=1;
        size2=s;


	if (size2%PAGE_SIZE>0) m=1;

	ptr_val_t cnt = size2/PAGE_SIZE+m;

	ptr_t ptr = NULL;
	if (t==AllocNormal) {
		void *avail = findAvail(size);
		if (avail!=NULL) ptr=(ptr_t)avail;
	}

	if (ptr==NULL) {
		if (m_lastPage==NULL || || m_freeTop==NULL || m_freeMax==NULL || ((ptr_val_t)m_freeTop+size2 >= (ptr_val_t)m_freeMax)) {
			// If remaining block is over sizeof(PtrInfo)+4 then we'll put it to the freed list
			if (m_lastPage!=NULL && m_freeTop!=NULL && m_freeMax!=NULL && (((ptr_val_t)m_freeTop+sizeof(PtrInfo)+sizeof(ptr_val_t)) < (ptr_val_t)m_freeMax)) {
				ptr_val_t diff = (ptr_val_t)m_freeMax-(ptr_val_t)m_freeTop;
				PtrInfo *nb = (PtrInfo*)m_freeTop;
				nb->prev = NULL;
				nb->next = NULL;
				nb->size = diff-sizeof(PtrInfo);
				nb->ressize = diff;
				ptr = (ptr_t)((ptr_val_t)nb+sizeof(PtrInfo));
				free(ptr, AllocDoNotLock);
			}
			ptr = (ptr_t)allocPage(cnt);
			if (ptr!=NULL) {
				m_lastPage = ptr;
				m_freeTop = ptr;
				m_freeMax = (void*)((ptr_val_t)ptr+(cnt*PAGE_SIZE));
			}
		} else {
			ptr = (ptr_t)m_freeTop;
		}
		if (m_freeTop!=NULL) {
			m_freeTop = (void*)((ptr_val_t)m_freeTop+size2);
		}
	}

	if (ptr==NULL) {
		//printf("=== ERROR: null page\n");
		return NULL;
	}

	PtrInfo *tmp = (PtrInfo*)ptr;
	tmp->prev = NULL;
	tmp->next = NULL;
	tmp->size = size;

	ptr = (ptr_t)((ptr_val_t)ptr + sizeof(PtrInfo));

	return ptr;
}

void *MM::allocMemClear(size_t size)
{
	unsigned char *ptr = (unsigned char*)allocMem(size, AllocNormal);
	unsigned char *tmp = ptr;

	if (ptr==NULL) return NULL;

	// This always ensures that the memory is cleared
#if 0
	for (size_t i=0; i<size; i++) {
		*tmp++ = 0;
	}
#else
	while (tmp<ptr+size) {
		*tmp++ = 0;
	}
#endif

	return (void*)ptr;
}

void *MM::alloc(size_t size, AllocType t)
{
	//if (size==0) return NULL;

	m.lock();
	if (size==0) size++;
	void *res = NULL;

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

bool MM::free(void *p, AllocLock l)
{
	if (p==NULL) return false;

	if (l==AllocDoLock) m.lock();
	PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p-sizeof(PtrInfo));

	tmp->prev = __mm_last_free;
	tmp->next = NULL;
	if (__mm_last_free!=NULL) {
		PtrInfo *last = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));

		tmp->prev = __mm_last_free;
		last->next = (ptr_t)p;
	}
	__mm_last_free = (ptr_t)p;

	if (l==AllocDoLock) m.unlock();

	return true;
}

void *MM::realloc(void *ptr, size_t size)
{
	if (size==0 && ptr!=NULL) {
		free(ptr);
		return NULL;
	}
	PtrInfo *old = NULL;
	if (ptr!=NULL) {
		old = (PtrInfo*)((ptr_val_t )ptr-sizeof(PtrInfo));
		if (old->size==size) return ptr;
	}

	void *tmp = malloc(size);
	if (tmp==NULL) return ptr;
	if (ptr==NULL) return tmp;

	m.lock();

	unsigned int min = old->size;
	if (min>size) min = size;

	char *dest = (char*)tmp;
	char *src = (char*)ptr;

	for (size_t i=0; i<min; i++) {
		*dest++ = *src++;
	}
	free(ptr, AllocDoNotLock);
	m.unlock();

	return tmp;
}

void *malloc(size_t size)
{
	return MM::instance()->alloc(size);
}

void *calloc(size_t cnt, size_t size)
{
	return MM::instance()->alloc(cnt*size, MM::AllocClear);
}

void *realloc(void *ptr, size_t size)
{
	return MM::instance()->realloc(ptr, size);
}

void free(void *ptr)
{
	MM::instance()->free(ptr);
}
