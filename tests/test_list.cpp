#include "test.h"
#include "../list.h"

int main(int argc, char **argv)
{
	TEST_INIT(list);

	List l;
	uint32_t val1 = 10;
	uint32_t val2 = 2005;
	uint32_t val3 = 3;
	uint32_t val4 = 42;
	uint32_t val5 = 123;
	uint32_t val6 = 666;
	uint32_t val7 = 72;

	TEST_ASSUME(list,l.size()==0);
	TEST_ASSUME(list,l.first()==NULL);
	TEST_ASSUME(list,l.last()==NULL);

	l.append(&val1);
	TEST_ASSUME(list,l.size()==1);
	TEST_ASSUME(list,l.first()==&val1);
	TEST_ASSUME(list,l.last()==&val1);

	l.append(&val2);
	TEST_ASSUME(list,l.size()==2);
	TEST_ASSUME(list,l.first()==&val1);
	TEST_ASSUME(list,l.last()==&val2);

	l.addFirst(&val3);
	TEST_ASSUME(list,l.size()==3);
	TEST_ASSUME(list,l.first()==&val3);
	TEST_ASSUME(list,l.last()==&val2);

	l.addFirst(&val4);
	TEST_ASSUME(list,l.size()==4);
	TEST_ASSUME(list,l.first()==&val4);
	TEST_ASSUME(list,l.last()==&val2);

	l.append(&val5);
	TEST_ASSUME(list,l.size()==5);
	TEST_ASSUME(list,l.first()==&val4);
	TEST_ASSUME(list,l.last()==&val5);

	l.appendAfter(2,&val6);
	TEST_ASSUME(list,l.size()==6);
	TEST_ASSUME(list,l.first()==&val4);
	TEST_ASSUME(list,l.last()==&val5);

	l.addAt(2,&val7);
	TEST_ASSUME(list,l.size()==7);
	TEST_ASSUME(list,l.first()==&val4);
	TEST_ASSUME(list,l.last()==&val5);
#if 0
	for (uint32_t i=0; i<7; i++) {
		printf("%x %d\n",l.at(i),*(uint32_t*)(l.at(i)));
	}
#endif

	TEST_ASSUME(list,l.at(0)==&val4);
	TEST_ASSUME(list,l.at(1)==&val3);
	TEST_ASSUME(list,l.at(2)==&val7);
	TEST_ASSUME(list,l.at(3)==&val1);
	TEST_ASSUME(list,l.at(4)==&val6);
	TEST_ASSUME(list,l.at(5)==&val2);
	TEST_ASSUME(list,l.at(6)==&val5);

	TEST_ASSUME(list,*(uint32_t*)l.at(0)==val4);
	TEST_ASSUME(list,*(uint32_t*)l.at(1)==val3);
	TEST_ASSUME(list,*(uint32_t*)l.at(2)==val7);
	TEST_ASSUME(list,*(uint32_t*)l.at(3)==val1);
	TEST_ASSUME(list,*(uint32_t*)l.at(4)==val6);
	TEST_ASSUME(list,*(uint32_t*)l.at(5)==val2);
	TEST_ASSUME(list,*(uint32_t*)l.at(6)==val5);


	TEST_END(list);
}
