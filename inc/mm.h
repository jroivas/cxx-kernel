#ifndef MM_H
#define MM_H

#ifdef __cplusplus

#include "mutex.hh"
#include "types.h"

struct PtrInfo;
class MM
{
public:
    enum AllocType {
        AllocNormal = 0,
        AllocFast,
        AllocClear,
        AllocPage
    };
    enum AllocLock {
        AllocDoNotLock = 0,
        AllocDoLock
    };

    MM();
    static MM *instance();
    void *alloc(size_t size, AllocType t=AllocNormal);
    bool free(void *p);
    void *realloc(void *ptr, size_t size);

private:
    void treeAdd(PtrInfo *p);
    void *releaseFree(PtrInfo *p, PtrInfo *parent);
    void *allocPage(size_t cnt);
    void *allocMem(size_t size, AllocType t);
    void *allocMemClear(size_t size);
    void *findAvail(size_t size);
    bool freeNoLock(void *p);
    void *m_lastPage;
    void *m_freeTop;
    void *m_freeMax;
};

extern "C" {
#endif

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t cnt, size_t size);
void *realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
