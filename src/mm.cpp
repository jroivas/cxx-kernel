#include "mm.h"

#ifdef USE_LINUX
#include <sys/mman.h>
#include <stdio.h>
#include <sys/time.h>
#else
#include "paging.h"
#include "arch/platform.h"
#endif
#include "string.hh"


static MM __mm_static_instance;
static volatile ptr_val_t  __mm_mutex;
static ptr_t __mm_last_free = nullptr;

#define SIZEMASK 0xFFFFFFFF

#ifdef USE_LINUX
#define PAGE_SIZE 4096
void getTiming(struct timeval *start)
{
    gettimeofday(start, nullptr);
}
double diffTiming(struct timeval *start, struct timeval *end)
{
    double tmp = end->tv_sec - start->tv_sec;
    tmp += ((double)end->tv_usec - (double)start->tv_usec)/1000000.0;
    return tmp;
}
#define TIME_START()\
    struct timeval __start;\
    getTiming(&__start);
#define TIME_END()\
    {\
        struct timeval __end;\
        getTiming(&__end);\
        double __time_diff = diffTiming(&__start,&__end);\
        if (__time_diff > 0.001) {\
            printf("%s time: %.8lf\n", __PRETTY_FUNCTION__, __time_diff);\
        }\
    }
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
    : m_lastPage(nullptr),
    m_freeTop(nullptr),
    m_freeMax(nullptr)
{
    m.assign(&__mm_mutex);
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
    block = mmap(nullptr, block_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (block == (void*)-1) {
        return nullptr;
    }

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
    while (p!=nullptr) {
        if (p->prev!=nullptr) {
            p = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
            s++;
        } else if (p->next!=nullptr) {
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
    if (p==nullptr) {
        printf("0\n");
    } else {
        padding("+",level);
        printf("%u\\ %p", (unsigned int)p->size, p);
        printf("\n");
//volatile unsigned long a=0;
//for (unsigned long kk=0; kk<0x4ffffff; kk++) a++;
//for (unsigned long kk=0; kk<0x0ffffff; kk++) a++;

        if (p->prev!=nullptr) {
                PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
                if (tmp!=p) {
                printf("L");
                travel(tmp, level+1);
                }
        }

        if (p->next!=nullptr) {
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
    if (cur == nullptr) return;

    ptr_t p = (ptr_t)((ptr_val_t)cur + sizeof(PtrInfo));

    if (__mm_last_free == nullptr) {
        __mm_last_free = (ptr_t)p;
    } else {
        PtrInfo *tmp = (PtrInfo*)((ptr_val_t)__mm_last_free - sizeof(PtrInfo));

        /* Setup a binary tree */
        while (tmp != nullptr) {
            if (cur->size < tmp->size) {
                if (tmp->prev == nullptr) {
                    tmp->prev = (ptr_t)p;
                    return;
                } else {
                    PtrInfo *tmp2 = (PtrInfo*)((ptr_val_t)tmp->prev - sizeof(PtrInfo));
#if 0
                    if (tmp2==tmp) {
                            printf("===A ERROR\n");
                            return;
                    }
#endif
                    tmp = tmp2;
                }
            } else {
                if (tmp->next == nullptr) {
                    tmp->next = (ptr_t)p;
                    return;
                } else {
                    PtrInfo *tmp2 = (PtrInfo*)((ptr_val_t)tmp->next - sizeof(PtrInfo));
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
    if (p == nullptr) {
        return nullptr;
    }
    ptr_t ptr = (ptr_t)((ptr_val_t)p + sizeof(PtrInfo));

#if 0
    printf("relfreepre %lu %p %p\n",p->size,p->prev,p->next);
    if (__mm_last_free!=nullptr) {
            PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
            travel(tmpr, 0);
    }
            printf("-\n");
#endif
    if (parent == nullptr) {
        if (p->prev == nullptr && p->next != nullptr) {
            __mm_last_free = p->next;
        } else if (p->prev != nullptr && p->next == nullptr) {
            __mm_last_free = p->prev;
        } else if (p->prev != nullptr && p->next != nullptr) {
            __mm_last_free = p->next;
            PtrInfo *tmp = (PtrInfo*)((ptr_val_t)p->prev-sizeof(PtrInfo));
            treeAdd(tmp);
        } else if (p->prev == nullptr && p->next == nullptr) {
            parent = nullptr;
        } else {
            printf("===0 ERROR\n");
        }
    } else if (p->prev == nullptr && p->next != nullptr) {
        if (parent->prev == ptr) {
            parent->prev = p->next;
        } else if (parent->next == ptr) {
            parent->next = p->next;
        } else {
            printf("===1 ERROR\n");
        }
    } else if (p->prev != nullptr && p->next == nullptr) {
        if (parent->prev==ptr) {
            parent->prev = p->prev;
        } else if (parent->next==ptr) {
            parent->next = p->prev;
        } else {
            printf("===2 ERROR\n");
        }
    } else if (p->prev!=nullptr && p->next!=nullptr) {
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
    } else if (p->prev==nullptr && p->next==nullptr) {
        if (parent->prev==ptr) {
            parent->prev = nullptr;
        } else if (parent->next==ptr) {
            parent->next = nullptr;
        } else {
            printf("===5 ERROR\n");
        }
    } else {
        printf("===4 ERROR\n");
    }
#if 0
    printf("relfree\n");
    if (__mm_last_free!=nullptr) {
        PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
        travel(tmpr, 0);
    }
        printf("-\n");
#endif
    p->next = nullptr;
    p->prev = nullptr;
    return (void*)p;
}

void *MM::findAvail(size_t size)
{
    if (__mm_last_free == nullptr) return nullptr;

    PtrInfo *tmp = (PtrInfo*)((ptr_val_t )__mm_last_free - sizeof(PtrInfo));
    PtrInfo *tmp2 = nullptr;

    while (tmp != nullptr) {
        if (tmp->size == size) {
            return releaseFree(tmp, tmp2);
        } else if (size < tmp->size) {
            if (tmp->prev != nullptr) {
                tmp2 = tmp;
                tmp = (PtrInfo*)((ptr_val_t)tmp->prev - sizeof(PtrInfo));
            } else if (tmp->next != nullptr
                && size <= ((PtrInfo*)((ptr_val_t)tmp->next-sizeof(PtrInfo)))->size) {
                tmp2 = tmp;
                tmp = (PtrInfo*)((ptr_val_t)tmp->next - sizeof(PtrInfo));
            } else {
                return nullptr;
            }
        } else if (size > tmp->size) {
            if (tmp->next != nullptr) {
                tmp2 = tmp;
                tmp = (PtrInfo*)((ptr_val_t)tmp->next - sizeof(PtrInfo));
            } else if (tmp->prev != nullptr
                && size >= ((PtrInfo*)((ptr_val_t)tmp->prev-sizeof(PtrInfo)))->size) {
                tmp2 = tmp;
                tmp = (PtrInfo*)((ptr_val_t)tmp->prev - sizeof(PtrInfo));
            } else {
                return nullptr;
            }
        }
    }

    return nullptr;
}

void *MM::allocMem(size_t size, AllocType t)
{
    ptr_val_t m = 0;

    ptr_val_t size2 = size + sizeof(PtrInfo);

#if 0
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
#endif

    if (size2%PAGE_SIZE!=0) m=1;
    ptr_val_t cnt = size2/PAGE_SIZE+m;


    ptr_t ptr = nullptr;
#if 1
    if (t == AllocNormal) {
        void *avail = findAvail(size);
        if (avail != nullptr) {
            ptr = (ptr_t)avail;
        }
    }
#else
    (void)t;
#endif

    if (ptr == nullptr) {
        if (m_lastPage == nullptr
            || m_freeTop == nullptr
            || m_freeMax == nullptr
            || ((ptr_val_t)m_freeTop+size2 >= (ptr_val_t)m_freeMax)) {
#if 1
            if (m_lastPage != nullptr
                && m_freeTop != nullptr
                && m_freeMax != nullptr
                && (((ptr_val_t)m_freeTop+sizeof(PtrInfo)+sizeof(ptr_val_t)) < (ptr_val_t)m_freeMax)) {

                ptr_val_t diff = (ptr_val_t)m_freeMax-(ptr_val_t)m_freeTop;
                PtrInfo *nb = reinterpret_cast<PtrInfo*>(m_freeTop);
                nb->prev = nullptr;
                nb->next = nullptr;
                nb->size = (unsigned int)(diff-sizeof(PtrInfo))&SIZEMASK;
                nb->ressize = (unsigned int)(diff&SIZEMASK);
                nb->state = PtrStateUsed;
                nb->used = 1;
                ptr = (ptr_t)((ptr_val_t)nb+sizeof(PtrInfo));
                free(ptr, AllocDoNotLock);
            }
#endif
            ptr = (ptr_t)allocPage(cnt);
            if (ptr == nullptr) {
                printf("=== ptr is null\n");
                return nullptr;
            }
            m_lastPage = ptr;
            m_freeTop = ptr;
            m_freeMax = (void*)((ptr_val_t)ptr + (cnt * PAGE_SIZE));
        } else {
            ptr = (ptr_t)m_freeTop;
        }
        if (m_freeTop != nullptr) {
            m_freeTop = (void*)((ptr_val_t)m_freeTop + size2);
        }
    }

    if (ptr == nullptr) {
        printf("=== ERROR: null chunk\n");
        return nullptr;
    }

    PtrInfo *tmp = reinterpret_cast<PtrInfo*>(ptr);
    tmp->prev = nullptr;
    tmp->next = nullptr;
    tmp->size = (unsigned int)(size & SIZEMASK);
    tmp->ressize = (unsigned int)(size2 & SIZEMASK);
    tmp->state = PtrStateUsed;
    tmp->used = 1;

    ptr = (ptr_t)((ptr_val_t)ptr + sizeof(PtrInfo));

    return ptr;
}

void *MM::allocMemClear(size_t size)
{
    unsigned char *ptr = (unsigned char*)allocMem(size, AllocNormal);
    unsigned char *tmp = ptr;

    if (ptr == nullptr) {
        return nullptr;
    }

    // This always ensures that the memory is cleared
    for (uint32_t i = 0; i < size; i++) {
        *(tmp + i) = 0;
    }

    return (void*)ptr;
}

void *MM::alloc(size_t size, AllocType t)
{
    //if (size == 0) return nullptr;
    if (size > SIZEMASK - sizeof(PtrInfo)) {
        return nullptr;
    }

    m.lock();
    void *res = nullptr;

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
    if (__mm_last_free!=nullptr) {
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
    if (p == nullptr) return false;

    if (l == AllocDoLock) m.lock();

    PtrInfo *cur = (PtrInfo*)((ptr_val_t)p - sizeof(PtrInfo));
    if (cur->used == 0) {
        //printf("===FFF Warn, probably not a valid block\n");
    }
    if (cur->state == PtrStateFreed) {
        //printf("===FF ERROR, double free\n");
        if (l==AllocDoLock) m.unlock();
        return false;
    }
    if (cur->state != PtrStateUsed) {
        //printf("===FD ERROR, invalid block: %d\n", cur->state);
        if (l==AllocDoLock) m.unlock();
        return false;
    }

    cur->prev = nullptr;
    cur->next = nullptr;
    cur->state = PtrStateFreed;
    cur->used = 0;

    treeAdd(cur);
#if 0
    //Travel
    if (__mm_last_free!=nullptr) {
        PtrInfo *tmpr = (PtrInfo*)((ptr_val_t)__mm_last_free-sizeof(PtrInfo));
        travel(tmpr, 0);
    }
        printf("\n");
#endif

    if (l == AllocDoLock) m.unlock();

    return true;
}

void *MM::realloc(void *ptr, size_t size)
{
    if (size == 0 && ptr != nullptr) {
        free(ptr);
        return nullptr;
    }

    PtrInfo *old = nullptr;
    if (ptr != nullptr) {
        old = (PtrInfo*)((ptr_val_t)ptr - sizeof(PtrInfo));
        if (old->size == size) {
            return ptr;
        }
        if (old->state == PtrStateFreed) {
            //printf("===FE WARNING, REALLOCING FREED\n");
            ptr = nullptr;
        }
    }

    void *tmp = alloc(size);
    // FIXME: This does not handle error properly
    if (tmp == nullptr) return ptr;
    if (ptr == nullptr) return tmp;

    m.lock();

    unsigned int min = old->size;
    if (min > size) {
        min = size;
    }

    Mem::move((char*)tmp, (char*)ptr, min);
    free(ptr, AllocDoNotLock);
    m.unlock();

    return tmp;
}

#ifndef ARCH_LINUX
void *malloc(size_t size)
{
    TIME_START();

    void *tmp = MM::instance()->alloc(size);

    TIME_END();

    return tmp;
}

void *calloc(size_t cnt, size_t size)
{
    TIME_START();

    void *tmp = MM::instance()->alloc(cnt*size, MM::AllocClear);

    TIME_END();

    return tmp;
}
void *realloc(void *ptr, size_t size)
{
    TIME_START();

    void *tmp = MM::instance()->realloc(ptr, size);

    TIME_END();

    return tmp;
}

void free(void *ptr)
{
    TIME_START();

    MM::instance()->free(ptr);

    TIME_END();
}
#endif
#undef printf
