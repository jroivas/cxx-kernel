#include "list.hh"

List::List()
{
    m_first = nullptr;
    m_last = nullptr;
    m_size = 0;
    m_freed_idx = 0;
}

List::~List()
{
    for (uint32_t i = 0; i < m_freed_idx; i++)
        delete m_freed[i];
}

List::ListObject *List::getFree(void *val)
{
    ListObject *res;

    if (m_freed_idx > 0) {
        res = m_freed[--m_freed_idx];
        res->assign(val);
    } else {
        res = new ListObject(val);
    }

    return res;
}

void List::free(ListObject *obj)
{
    if (m_freed_idx + 1 >= LIST_MAX_FREED) {
        delete obj;
        return;
    }
    m_freed[m_freed_idx++] = obj;
}

void List::addFirst(void *val)
{
    ListObject *obj = getFree(val);
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

    ListObject *obj = getFree(val);
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

void List::rotateFirstLast()
{
    if (m_first == nullptr)
        return;

    ListObject *tmp = m_first;
    if (tmp == m_last)
        return;

    m_first = tmp->next;
    m_last->next = tmp;
    tmp->next = nullptr;
    m_last = tmp;
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
    free(tmp);
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
            free(obj);
            obj = prev->next;
        }
        prev = obj;
        if (obj != nullptr) {
            obj = obj->next;
        }
    }
}

uint32_t List::size() const
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

    ListObject *another_obj = getFree(val);
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

    ListObject *another_obj = getFree(val);
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
        free(tmp);
    }
    else {
        m_first = nullptr;
        m_last = nullptr;
        m_size = 0;
        free(obj);
    }

    return true;
}
