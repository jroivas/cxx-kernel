#include "test.h"
#include <list.h>

int main(int argc, char **argv)
{
    TEST_INIT(list);

    List l;
    List p;
    uint32_t val1 = 10;
    uint32_t val2 = 2005;
    uint32_t val3 = 3;
    uint32_t val4 = 42;
    uint32_t val5 = 123;
    uint32_t val6 = 666;
    uint32_t val7 = 72;

    TEST_ASSUME(list,l.size()==0);
    TEST_ASSUME(list,l.first()==nullptr);
    TEST_ASSUME(list,l.last()==nullptr);

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

    l.append(&val7);
    TEST_ASSUME(list,l.size()==8);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val7);

    l.deleteAll(&val7);
    TEST_ASSUME(list,l.size()==6);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val5);

    l.addAt(2,&val7);
    TEST_ASSUME(list,l.size()==7);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val5);

    l.deleteAt(0);
    TEST_ASSUME(list,l.size()==6);
    TEST_ASSUME(list,l.first()==&val3);
    TEST_ASSUME(list,l.last()==&val5);

    l.deleteAt(5);
    TEST_ASSUME(list,l.size()==5);
    TEST_ASSUME(list,l.first()==&val3);
    TEST_ASSUME(list,l.last()==&val2);

    l.addAt(0,&val4);
    TEST_ASSUME(list,l.size()==6);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val2);

    l.append(&val5);
    TEST_ASSUME(list,l.size()==7);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val5);

    l.append(&val4);
    TEST_ASSUME(list,l.size()==8);
    TEST_ASSUME(list,l.first()==&val4);
    TEST_ASSUME(list,l.last()==&val4);

    void *tmp = l.takeFirst();
    TEST_ASSUME(list,l.size()==7);
    TEST_ASSUME(list,(uint32_t*)tmp==&val4);
    TEST_ASSUME(list,*(uint32_t*)tmp==val4);
    TEST_ASSUME(list,l.first()==&val3);
    TEST_ASSUME(list,l.last()==&val4);

#if 0
    for (uint32_t i=0; i<l.size(); i++) {
            printf("%x %d\n",l.at(i),*(uint32_t*)(l.at(i)));
    }
#endif

    TEST_ASSUME(list,l.at(0)==&val3);
    TEST_ASSUME(list,l.at(1)==&val7);
    TEST_ASSUME(list,l.at(2)==&val1);
    TEST_ASSUME(list,l.at(3)==&val6);
    TEST_ASSUME(list,l.at(4)==&val2);
    TEST_ASSUME(list,l.at(5)==&val5);
    TEST_ASSUME(list,l.at(6)==&val4);

    TEST_ASSUME(list,*(uint32_t*)l.at(0)==val3);
    TEST_ASSUME(list,*(uint32_t*)l.at(1)==val7);
    TEST_ASSUME(list,*(uint32_t*)l.at(2)==val1);
    TEST_ASSUME(list,*(uint32_t*)l.at(3)==val6);
    TEST_ASSUME(list,*(uint32_t*)l.at(4)==val2);
    TEST_ASSUME(list,*(uint32_t*)l.at(5)==val5);
    TEST_ASSUME(list,*(uint32_t*)l.at(6)==val4);


    p.append(&val1);
    TEST_ASSUME(list,p.size()==1);
    TEST_ASSUME(list,p.first()==&val1);
    TEST_ASSUME(list,p.last()==&val1);

    p.append(&val2);
    TEST_ASSUME(list,p.size()==2);
    TEST_ASSUME(list,p.first()==&val1);
    TEST_ASSUME(list,p.last()==&val2);

    TEST_ASSUME(list,p.takeFirst()==&val1);
    TEST_ASSUME(list,p.size()==1);
    TEST_ASSUME(list,p.first()==&val2);
    TEST_ASSUME(list,p.last()==&val2);

    p.append(&val3);
    TEST_ASSUME(list,p.size()==2);
    TEST_ASSUME(list,p.first()==&val2);
    TEST_ASSUME(list,p.last()==&val3);

    TEST_ASSUME(list,p.takeFirst()==&val2);
    TEST_ASSUME(list,p.size()==1);
    TEST_ASSUME(list,p.first()==&val3);
    TEST_ASSUME(list,p.last()==&val3);

    TEST_ASSUME(list,p.takeFirst()==&val3);
    TEST_ASSUME(list,p.size()==0);
    TEST_ASSUME(list,p.first()==nullptr);
    TEST_ASSUME(list,p.last()==nullptr);

    TEST_ASSUME(list,p.takeFirst()==nullptr);
    TEST_ASSUME(list,p.size()==0);
    TEST_ASSUME(list,p.first()==nullptr);
    TEST_ASSUME(list,p.last()==nullptr);
    TEST_END(list);
}
