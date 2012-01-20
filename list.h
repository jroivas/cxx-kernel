#ifndef _LIST_H
#define _LIST_H

#include "types.h"

class List
{
public:
	List();
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

protected:
	class ListObject {
	public:
		ListObject() { next = NULL; ptr = NULL; }
		ListObject(void *val) { ptr = val; next = NULL; }
		ListObject *next;
		void *ptr;
	};
	ListObject *m_first;
	ListObject *m_last;
};
#endif
