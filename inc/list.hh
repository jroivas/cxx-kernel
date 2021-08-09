#ifndef _LIST_HH
#define _LIST_HH

#include "types.h"

#define LIST_MAX_FREED 32

class List
{
public:
    class ListObject {
        friend class List;
    public:
        ListObject()
        {
            next = nullptr;
            ptr = nullptr;
        }
        ListObject(void *val)
        {
            ptr = val;
            next = nullptr;
        }
        void assign(void *val)
        {
            ptr = val;
            next = nullptr;
        }

    protected:
        ListObject *next;
        void *ptr;
    };

    List();
    ~List();
    void addFirst(void *val);
    void append(void *val);
    void *first();
    void *last();
    void deleteAll(void *val);
    void *takeFirst();

    uint32_t size();
    void *at(uint32_t i);
    bool appendAfter(uint32_t i, void *val);
    bool addAt(uint32_t i, void *val);
    bool deleteAt(uint32_t i);

    const List::ListObject *begin() const;
    const List::ListObject *next(const List::ListObject *iter) const;
    const void *item(const List::ListObject *iter) const;

protected:
    ListObject *m_first;
    ListObject *m_last;
    uint32_t m_size;
    ListObject *getFree(void *val);
    void free(ListObject *);
    ListObject *m_freed[LIST_MAX_FREED];
    uint32_t m_freed_idx;
};
#endif
