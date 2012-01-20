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

void *List::last()
{
	if (m_last==NULL) return NULL;
	return m_last->ptr;
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
	}
	o->next = obj;

	return true;
}

bool List::addAt(uint32_t i, void *val)
{
	if (val==NULL) return false;

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
