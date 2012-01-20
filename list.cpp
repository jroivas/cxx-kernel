#include "list.h"

List::List()
{
	m_first = NULL;
	m_last = NULL;
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
}

void List::append(void *val)
{
	if (m_last==NULL) {
		addFirst(val);
		return;
	}

	ListObject *obj = new ListObject(val);
	ListObject *tmp = m_last;
	tmp->next = obj;
	m_last = obj;
}

void *List::first()
{
	if (m_first==NULL) return NULL;
	return m_first->ptr;
}

void *List::takeFirst()
{
	if (m_first==NULL) return NULL;

	ListObject *tmp = m_first;
	if (tmp->next!=NULL) {
		m_first = tmp->next;
	} else {
		m_first = NULL;
		m_last = NULL;
	}
	void *val = tmp->ptr;
	delete tmp;
	return val;
}

void *List::last()
{
	if (m_last==NULL) return NULL;
	return m_last->ptr;
}

void List::deleteAll(void *val)
{
	if (m_first==NULL) return;

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
			delete o;
			o = prev->next;
		}
		prev = o;
		if (o!=NULL) o = o->next;
	}
}

uint32_t List::size()
{
	if (m_first==NULL) return 0;

	uint32_t c = 1;
	ListObject *o = m_first;
	while (o->next!=NULL) {
		o = o->next;
		c++;
	}

	return c;
}

void *List::at(uint32_t i)
{
	ListObject *o = m_first;
	uint32_t c = 0;

	if (o==NULL) return NULL;
	while (c<i && o->next!=NULL) {
		c++;
		o = o->next;
	}
	if (c!=i) return NULL;

	return o->ptr;
}

bool List::appendAfter(uint32_t i, void *val)
{
	if (val==NULL) return false;
	if (m_first==NULL) {
		addFirst(val);
		return true;
	}

	ListObject *o = m_first;
	uint32_t c = 0;

	if (o==NULL) return NULL;
	while (c<i && o->next!=NULL) {
		c++;
		o = o->next;
	}

	ListObject *obj = new ListObject(val);
	if (o->next!=NULL) {
		obj->next = o->next;
	} else {
		m_last = obj;
	}
	o->next = obj;

	return true;
}

bool List::addAt(uint32_t i, void *val)
{
	if (val==NULL) return false;
	if (m_first==NULL || i==0) {
		addFirst(val);
		return true;
	}

	ListObject *o = m_first;
	uint32_t c = 0;

	if (o==NULL) return NULL;
	while (c+1<i && o->next!=NULL) {
		c++;
		o = o->next;
	}

	ListObject *obj = new ListObject(val);
	if (o->next!=NULL) {
		obj->next = o->next;
	}
	o->next = obj;

	return true;
}

bool List::deleteAt(uint32_t i)
{
	if (m_first==NULL) return false;

	if (i==0) {
		if (m_first->next==NULL) {
			m_first = NULL;
			m_last = NULL;
		} else {
			m_first = m_first->next;
		}
		return true;
	}

	ListObject *o = m_first;
	uint32_t c = 0;

	if (o==NULL) return NULL;
	while (c+1<i && o->next!=NULL) {
		c++;
		o = o->next;
	}
	if (c+1!=i) return false;

	if (o->next!=NULL) {
		ListObject *tmp = o->next;
		o->next = tmp->next;
		if (tmp==m_last) m_last = o->next;
		if (tmp->next==NULL) m_last = o;
		delete tmp;
	}
	else {
		m_first = NULL;
		m_last = NULL;
		delete o;
	}

	return true;
}
