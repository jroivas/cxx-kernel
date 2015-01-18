#ifndef _TEST_H
#define _TEST_H

#define ARCH_LINUX

#include <stdio.h>

#define TEST_INIT(n)\
    static unsigned int __test_cnt_##n = 0;\
    static unsigned int __test_fail_cnt_##n = 0;
#define TEST_ASSUME(n,x)\
    __test_cnt_##n++;\
    if (!(x)) {\
        printf("ERROR on %s @%d %s: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__,#x);\
        __test_fail_cnt_##n++;\
    }
#define TEST_END(n)\
    if (__test_fail_cnt_##n>0) {\
        printf("%s: %d/%d FAILED\n",#n,__test_fail_cnt_##n,__test_cnt_##n);\
        return 1;\
    } else {\
        printf("%s: %d OK\n",#n,__test_cnt_##n);\
        return 0;\
    }

#endif
