#include "list.hh"

List::List()
{
    m_first = nullptr;
    m_last = nullptr;
    m_size = 0;
}

void List::addFirst(void *val)
{
    ListObject *obj = new ListObject(val);
    if (m_first != nullptr) {
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
    if (m_last == nullptr) {
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
    if (m_first == nullptr) return nullptr;
    return m_first->ptr;
}

void *List::takeFirst()
{
    if (m_first == nullptr) return nullptr;

    ListObject *tmp = m_first;
    if (tmp->next != nullptr) {
        m_first = tmp->next;
    } else {
        m_first = nullptr;
        m_last = nullptr;
    }
    void *val = tmp->ptr;
    m_size--;
    delete tmp;
    return val;
}

void *List::last()
{
    if (m_last == nullptr) return nullptr;
    return m_last->ptr;
}

void List::deleteAll(void *val)
{
    if (m_first == nullptr) return;

    ListObject *obj = m_first;
    ListObject *prev = nullptr;

    while (obj != nullptr) {
        if (obj->ptr == val) {
            if (prev == nullptr) {
                m_first = obj->next;
            } else {
                prev->next = obj->next;
            }
            if (m_last == obj) {
                m_last = prev;
                prev->next = nullptr;
            }
            m_size--;
            delete obj;
            obj = prev->next;
        }
        prev = obj;
        if (obj != nullptr) {
            obj = obj->next;
        }
    }
}

uint32_t List::size()
{
    return m_size;
}

const List::ListObject *List::begin() const
{
    return m_first;
}

const List::ListObject *List::next(const List::ListObject *iter) const
{
    if (iter == nullptr) {
        return nullptr;
    }
    return iter->next;
}

const void *List::item(const List::ListObject *iter) const
{
    if (iter == nullptr) {
        return nullptr;
    }
    return iter->ptr;
}

void *List::at(uint32_t i)
{
    ListObject *obj = m_first;
    if (obj == nullptr) {
        return nullptr;
    }

    uint32_t c = 0;
    while (c < i && obj->next != nullptr) {
        ++c;
        obj = obj->next;
    }
    if (c != i) {
        return nullptr;
    }

    return obj->ptr;
}

bool List::appendAfter(uint32_t i, void *val)
{
    if (val == nullptr) {
        return false;
    }
    if (m_first == nullptr) {
        addFirst(val);
        return true;
    }

    ListObject *obj = m_first;
    uint32_t c = 0;

    if (obj == nullptr) {
        return false;
    }
    while (c < i && obj->next != nullptr) {
        c++;
        obj = obj->next;
    }

    ListObject *another_obj = new ListObject(val);
    if (obj->next != nullptr) {
        another_obj->next = obj->next;
    } else {
        m_last = another_obj;
    }
    obj->next = another_obj;
    m_size++;

    return true;
}

bool List::addAt(uint32_t i, void *val)
{
    if (val == nullptr) {
        return false;
    }
    if (m_first == nullptr || i == 0) {
        addFirst(val);
        return true;
    }

    ListObject *obj = m_first;
    uint32_t c = 0;

    if (obj == nullptr) {
        return false;
    }
    while (c + 1 < i && obj->next != nullptr) {
        c++;
        obj = obj->next;
    }

    ListObject *another_obj = new ListObject(val);
    if (obj->next != nullptr) {
        another_obj->next = obj->next;
    }
    obj->next = another_obj;
    m_size++;

    return true;
}

bool List::deleteAt(uint32_t i)
{
    if (m_first == nullptr) {
        return false;
    }

    if (i == 0) {
        if (m_first->next==nullptr) {
            m_first = nullptr;
            m_last = nullptr;
            m_size = 0;
        } else {
            m_first = m_first->next;
            m_size--;
        }
        return true;
    }

    ListObject *obj = m_first;
    uint32_t c = 0;

    if (obj == nullptr) {
        return false;
    }
    while (c + 1 < i && obj->next != nullptr) {
        c++;
        obj = obj->next;
    }
    if ((c + 1) != i) {
        return false;
    }

    if (obj->next != nullptr) {
        ListObject *tmp = obj->next;
        obj->next = tmp->next;
        if (tmp == m_last) {
            m_last = obj->next;
        }
        if (tmp->next == nullptr) {
            m_last = obj;
        }
        m_size--;
        delete tmp;
    }
    else {
        m_first = nullptr;
        m_last = nullptr;
        m_size = 0;
        delete obj;
    }

    return true;
}
