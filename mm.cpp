#include "mm.h"

#ifdef USE_LINUX
#include <sys/mman.h>
#include <stdio.h>
#include <sys/time.h>
#else
#include "paging.h"
#include "arch/platform.h"
#endif
       

static MM __mm_static_instance;
static volatile ptr_val_t  __mm_mutex;
static ptr_t __mm_last_free = NULL;

#define SIZEMASK 0xFFFFFFFF

#ifdef USE_LINUX
#define PAGE_SIZE 4096
void getTiming(struct timeval *start)
{
	gettimeofday(start, NULL);
}
double diffTiming(struct timeval *start, struct timeval *end)
{
	double tmp = end->tv_sec - start->tv_sec;
	tmp += ((double)end->tv_usec - (double)start->tv_usec)/1000000.0;
	return tmp;
}
#define TIME_START() struct timeval start; getTiming(&start);
#define TIME_END() { struct timeval end; getTiming(&end); double diffi=diffTiming(&start,&end); if (diffi>0.001) printf("%s time: %.8lf\n",__PRETTY_FUNCTION__, diffi); }
#else
#define TIME_START()
#define TIME_END()
void printf(...) {}
#endif

enum PtrState
{
	PtrStateNone = 0,
	PtrStateUsed = 0x7E,
	PtrStateFreed = 0x42
};

struct PtrInfo
{
	ptr_t prev;
	ptr_t next;
#if 0
	ptr_val_t size;
	ptr_val_t ressize;
#else
	unsigned int size;
	unsigned int ressize;
#endif
	PtrState state:7;
	char used:1;
};

MM::MM()
{
	m.assign(&__mm_mutex);
	m_lastPage = NULL;
	m_freeTop = NULL;
	m_freeMax = NULL;
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
	//block = mmap((void *)0, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	block = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block==(void*)-1) return NULL;

	return block;
#else
	Paging p;
	p.lock();
	void *tmp = p.alloc(cnt);
	p.unlock();
	return tmp;
#endif
}

#if 0
void padding(const char *ch, int n)
{
	for (int i=0; i<n; i++) {
		printf("%s",ch);
	}
}

int depth(PtrInfo *p)
{
	int s = 0;
	while (p!=NULL) {
		if (p->prev!=NULL) {
			p = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
			s++;
		} else if (p->next!=NULL) {
			p = (PtrInfo*)((ptr_val_t)p->next-sizeof(PtrInfo));
			s++;
		} else {
			break;
		}
	}
	return s;
}

void travel(PtrInfo *p, int level)
{
	if (p==NULL) {
		printf("0\n");
	} else {
		padding("+",level);
		printf("%u\\ %p", (unsigned int)p->size, p);
		printf("\n");
//	volatile unsigned long a=0;
//	for (unsigned long kk=0; kk<0x4ffffff; kk++) a++;
//	for (unsigned long kk=0; kk<0x0ffffff; kk++) a++;

		if (p->prev!=NULL) {
			PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
			if (tmp!=p) {
			printf("L");
			travel(tmp, level+1);
			}
		}

		if (p->next!=NULL) {
			PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->next-sizeof(PtrInfo));
			if (tmp!=p) {
			printf("R");
			travel(tmp, level+1);
			}
		}
	}
}
#endif

void MM::treeAdd(PtrInfo *cur)
{
	if (cur==NULL) return;

	ptr_t p = (ptr_t)((ptr_val_t)cur+sizeof(PtrInfo));

	if (__mm_last_free==NULL) {
		__mm_last_free = (ptr_t)p;
	} else {
		PtrInfo *tmp = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));

		/* Setup a binary tree */
		while (tmp!=NULL) {
			if (cur->size < tmp->size) {
				if (tmp->prev == NULL) {
					tmp->prev = (ptr_t)p;
					return;
				} else {
					PtrInfo *tmp2 = (PtrInfo*)((ptr_val_t)tmp->prev-sizeof(PtrInfo));
#if 0
					if (tmp2==tmp) {
						printf("===A ERROR\n");
						return;
					}
#endif
					tmp=tmp2;
				}
			} else if (cur->size >= tmp->size) {
				if (tmp->next == NULL) {
					tmp->next = (ptr_t)p;
					return;
				} else {
					PtrInfo *tmp2 = (PtrInfo*)((ptr_val_t)tmp->next-sizeof(PtrInfo));
#if 0
					if (tmp2==tmp) {
						printf("===B ERROR %u %p\n",(unsigned int)tmp->size,tmp->next);
						return;
					}
#endif
					tmp = tmp2;
				}
			}
		}
	}
}

void *MM::releaseFree(PtrInfo *p, PtrInfo *parent)
{
	if (p==NULL) return NULL;
	ptr_t ptr = (ptr_t)((ptr_val_t)p+sizeof(PtrInfo));

#if 0
	printf("relfreepre %lu %p %p\n",p->size,p->prev,p->next);
	if (__mm_last_free!=NULL) {
		PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
		travel(tmpr, 0);
	}
		printf("-\n");
#endif
	if (parent==NULL) {
		if (p->prev==NULL && p->next!=NULL) {
			__mm_last_free = p->next;
		} else if (p->prev!=NULL && p->next==NULL) {
			__mm_last_free = p->prev;
		} else if (p->prev!=NULL && p->next!=NULL) {
			__mm_last_free = p->next;
			PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
			treeAdd(tmp);
		} else if (p->prev==NULL && p->next==NULL) {
			parent = NULL;
		} else {
			printf("===0 ERROR\n");
		}
	} else if (p->prev==NULL && p->next!=NULL) {
		if (parent->prev==ptr) {
			parent->prev = p->next;
		} else if (parent->next==ptr) {
			parent->next = p->next;
		} else {
			printf("===1 ERROR\n");
		}
	} else if (p->prev!=NULL && p->next==NULL) {
		if (parent->prev==ptr) {
			parent->prev = p->prev;
		} else if (parent->next==ptr) {
			parent->next = p->prev;
		} else {
			printf("===2 ERROR\n");
		}
	} else if (p->prev!=NULL && p->next!=NULL) {
		if (parent->prev==ptr) {
			PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->next-sizeof(PtrInfo));
			parent->prev = p->prev;
			treeAdd(tmp);
		} else if (parent->next==ptr) {
			PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
			parent->next = p->next;
			treeAdd(tmp);
		} else {
			printf("===3 ERROR\n");
		}
	} else if (p->prev==NULL && p->next==NULL) {
		if (parent->prev==ptr) {
			parent->prev = NULL;
		} else if (parent->next==ptr) {
			parent->next = NULL;
		} else {
			printf("===5 ERROR\n");
		}
	} else {
		printf("===4 ERROR\n");
	}
#if 0
	printf("relfree\n");
	if (__mm_last_free!=NULL) {
		PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
		travel(tmpr, 0);
	}
		printf("-\n");
#endif
	p->next = NULL;
	p->prev = NULL;
	return (void*)p;
}

void *MM::findAvail(size_t size)
{
	if (__mm_last_free==NULL) return NULL;

	PtrInfo *tmp = (PtrInfo*)((ptr_val_t )__mm_last_free-sizeof(PtrInfo));
	PtrInfo *tmp2 = NULL;

	while (tmp!=NULL) {
		if (tmp->size == size) {
			return releaseFree(tmp, tmp2);
		} else if (size < tmp->size) {
			if (tmp->prev!=NULL) {
				tmp2 = tmp;
				tmp = (PtrInfo*)((ptr_val_t)tmp->prev-sizeof(PtrInfo));
			} else if (tmp->next!=NULL && size <= ((PtrInfo*)((ptr_val_t)tmp->next-sizeof(PtrInfo)))->size) {
				tmp2 = tmp;
				tmp = (PtrInfo*)((ptr_val_t)tmp->next-sizeof(PtrInfo));
			} else {
				return NULL;
			}
		} else if (size > tmp->size) {
			if (tmp->next!=NULL) {
				tmp2 = tmp;
				tmp = (PtrInfo*)((ptr_val_t)tmp->next-sizeof(PtrInfo));
			} else if (tmp->prev!=NULL && size >= ((PtrInfo*)((ptr_val_t)tmp->prev-sizeof(PtrInfo)))->size) {
				tmp2 = tmp;
				tmp = (PtrInfo*)((ptr_val_t)tmp->prev-sizeof(PtrInfo));

			} else {
				return NULL;
			}
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
#if 1
	if (t==AllocNormal) {
		void *avail = findAvail(size);
		if (avail!=NULL) ptr=(ptr_t)avail;
	}
#else
	(void)t;
#endif

	if (ptr==NULL) {
		if (m_lastPage==NULL || m_freeTop==NULL || m_freeMax==NULL || ((ptr_val_t)m_freeTop+size2 >= (ptr_val_t)m_freeMax)) {
#if 1
			if (m_lastPage!=NULL && m_freeTop!=NULL && m_freeMax!=NULL && (((ptr_val_t)m_freeTop+sizeof(PtrInfo)+sizeof(ptr_val_t)) < (ptr_val_t)m_freeMax)) {
				ptr_val_t diff = (ptr_val_t)m_freeMax-(ptr_val_t)m_freeTop;
				PtrInfo *nb = (PtrInfo*)m_freeTop;
				nb->prev = NULL;
				nb->next = NULL;
				nb->size = (unsigned int)(diff-sizeof(PtrInfo))&SIZEMASK;
				nb->ressize = (unsigned int)(diff&SIZEMASK);
				nb->state = PtrStateUsed;
				nb->used = 1;
				ptr = (ptr_t)((ptr_val_t)nb+sizeof(PtrInfo));
				free(ptr, AllocDoNotLock);
			}
#endif
			ptr = (ptr_t)allocPage(cnt);
			if (ptr==NULL) {
				printf("=== ptr is null\n");
				return NULL;
			}
			m_lastPage = ptr;
			m_freeTop = ptr;
			m_freeMax = (void*)((ptr_val_t)ptr+(cnt*PAGE_SIZE));
		} else {
			ptr = (ptr_t)m_freeTop;
		}
		if (m_freeTop!=NULL) {
			m_freeTop = (void*)((ptr_val_t)m_freeTop+size2);
		}
	}

	if (ptr==NULL) {
		printf("=== ERROR: null chunk\n");
		return NULL;
	}

	//Platform::video()->printf("allocMem 3\n");
	PtrInfo *tmp = (PtrInfo*)ptr;
	tmp->prev = NULL;
	tmp->next = NULL;
	tmp->size = (unsigned int)(size&SIZEMASK);
	tmp->ressize = (unsigned int)(size2&SIZEMASK);
	tmp->state = PtrStateUsed;
	tmp->used = 1;

	ptr = (ptr_t)((ptr_val_t)ptr+sizeof(PtrInfo));

	return ptr;
}

void *MM::allocMemClear(size_t size)
{
	unsigned char *ptr = (unsigned char*)allocMem(size, AllocNormal);
	unsigned char *tmp = ptr;

	if (ptr==NULL) return NULL;

	// This always ensures that the memory is cleared
	for (uint32_t i=0; i<size; i++) {
		*(tmp+i) = 0;
	}

	return (void*)ptr;
}

void *MM::alloc(size_t size, AllocType t)
{
	//if (size==0) return NULL;
	if (size>SIZEMASK-sizeof(PtrInfo)) return NULL;

	m.lock();
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
#if 0
	//Travel
	if (__mm_last_free!=NULL) {
		PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
		travel(tmpr, 0);
	}
		printf("\n");
#endif
	m.unlock();
	return res;
}

bool MM::free(void *p, AllocLock l)
{
	if (p==NULL) return false;

	if (l==AllocDoLock) m.lock();

	PtrInfo *cur = (PtrInfo*)((ptr_val_t)p-sizeof(PtrInfo));
	if (cur->used==0) {
		printf("===FFF Warn, probably not a valid block\n");
	}
	if (cur->state == PtrStateFreed) {
		printf("===FF ERROR, double free\n");
		if (l==AllocDoLock) m.unlock();
		return false;
	}
	if (cur->state != PtrStateUsed) {
		printf("===FD ERROR, invalid block: %d\n", cur->state);
		if (l==AllocDoLock) m.unlock();
		return false;
	}

	cur->prev = NULL;
	cur->next = NULL;
	cur->state = PtrStateFreed;
	cur->used = 0;

	treeAdd(cur);
#if 0
	//Travel
	if (__mm_last_free!=NULL) {
		PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
		travel(tmpr, 0);
	}
		printf("\n");
#endif

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
		old = (PtrInfo*)((ptr_val_t)ptr-sizeof(PtrInfo));
		if (old->size==size) return ptr;
		if (old->state == PtrStateFreed) {
			printf("===FE WARNING, REALLOCING FREED\n");
			ptr = NULL;
		}
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
	//printf("malloc %lu\n",size);
	TIME_START();
	void *tmp;
	tmp = MM::instance()->alloc(size);
	TIME_END();
	return tmp;
	//return MM::instance()->alloc(size);
}

void *calloc(size_t cnt, size_t size)
{
	//return MM::instance()->alloc(cnt*size, MM::AllocClear);
	TIME_START();
	void *tmp;
	tmp = MM::instance()->alloc(cnt*size, MM::AllocClear);
	TIME_END();
	return tmp;
}
void *realloc(void *ptr, size_t size)
{
//	printf("realloc\n");
	TIME_START();
	void *tmp;
	tmp = MM::instance()->realloc(ptr, size);
	TIME_END();
	return tmp;
	//return MM::instance()->realloc(ptr, size);
}

void free(void *ptr)
{
//	printf("free\n");
	TIME_START();
	MM::instance()->free(ptr);
	TIME_END();
}
#undef printf
