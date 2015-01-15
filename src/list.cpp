#include "list.h"

List::List()
{
    m_first = NULL;
    m_last = NULL;
    m_size = 0;
}

void List::addFirst(void *val)
{
    ListObject *obj = new ListObject(val);
    if (m_first != NULL) {
        ListObject *tmp = m_first;
        obj->next = tmp;
    } else {
        m_last = obj;
    }
    m_first = obj;
    m_size++;
}

void List::append(void *val)
{
    if (m_last == NULL) {
        addFirst(val);
        return;
    }

    ListObject *obj = new ListObject(val);
    ListObject *tmp = m_last;
    tmp->next = obj;
    m_last = obj;
    m_size++;
}

void *List::first()
{
    if (m_first == NULL) return NULL;
    return m_first->ptr;
}

void *List::takeFirst()
{
    if (m_first == NULL) return NULL;

    ListObject *tmp = m_first;
    if (tmp->next != NULL) {
        m_first = tmp->next;
    } else {
        m_first = NULL;
        m_last = NULL;
    }
    void *val = tmp->ptr;
    m_size--;
    delete tmp;
    return val;
}

void *List::last()
{
    if (m_last == NULL) return NULL;
    return m_last->ptr;
}

void List::deleteAll(void *val)
{
    if (m_first == NULL) return;

    ListObject *o = m_first;
    ListObject *prev = NULL;
    while (o!=NULL) {
        if (o->ptr == val) {
            if (prev==NULL) {
                m_first = o->next;
            } else {
                prev->next = o->next;
            }
            if (m_last == o) {
                m_last = prev;
                prev->next = NULL;
            }
            m_size--;
            delete o;
            o = prev->next;
        }
        prev = o;
        if (o!=NULL) o = o->next;
    }
}

uint32_t List::size()
{
#if 0
    if (m_first==NULL) return 0;

    uint32_t c = 1;
    ListObject *o = m_first;
    while (o->next!=NULL) {
        o = o->next;
        c++;
    }

    return c;
#else
    return m_size;
#endif
}

void *List::at(uint32_t i)
{
    ListObject *o = m_first;
    uint32_t c = 0;

    if (o == NULL) return NULL;
    while (c < i && o->next != NULL) {
        c++;
        o = o->next;
    }
    if (c!=i) return NULL;

    return o->ptr;
}

bool List::appendAfter(uint32_t i, void *val)
{
    if (val == NULL) return false;
    if (m_first == NULL) {
        addFirst(val);
        return true;
    }

    ListObject *o = m_first;
    uint32_t c = 0;

    if (o == NULL) return NULL;
    while (c<i && o->next!=NULL) {
        c++;
        o = o->next;
    }

    ListObject *obj = new ListObject(val);
    if (o->next != NULL) {
        obj->next = o->next;
    } else {
        m_last = obj;
    }
    o->next = obj;
    m_size++;

    return true;
}

bool List::addAt(uint32_t i, void *val)
{
    if (val == NULL) return false;
    if (m_first == NULL || i == 0) {
        addFirst(val);
        return true;
    }

    ListObject *o = m_first;
    uint32_t c = 0;

    if (o==NULL) return NULL;
    while (c + 1 < i && o->next != NULL) {
        c++;
        o = o->next;
    }

    ListObject *obj = new ListObject(val);
    if (o->next!=NULL) {
        obj->next = o->next;
    }
    o->next = obj;
    m_size++;

    return true;
}

bool List::deleteAt(uint32_t i)
{
    if (m_first == NULL) return false;

    if (i == 0) {
        if (m_first->next==NULL) {
            m_first = NULL;
            m_last = NULL;
            m_size = 0;
        } else {
            m_first = m_first->next;
            m_size--;
        }
        return true;
    }

    ListObject *o = m_first;
    uint32_t c = 0;

    if (o == NULL) return NULL;
    while (c + 1 < i && o->next != NULL) {
        c++;
        o = o->next;
    }
    if (c+1!=i) return false;

    if (o->next != NULL) {
        ListObject *tmp = o->next;
        o->next = tmp->next;
        if (tmp == m_last) m_last = o->next;
        if (tmp->next == NULL) m_last = o;
        m_size--;
        delete tmp;
    }
    else {
        m_first = NULL;
        m_last = NULL;
        m_size = 0;
        delete o;
    }

    return true;
}
