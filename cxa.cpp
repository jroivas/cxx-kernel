#include "cxa.h"

#ifndef ARCH_LINUX
struct CXA
{
	void (*d)(void *);
	void *arg;
	void *dso;
};

#define CXA_MAX_SIZE 256

static CXA __cxa_funcs[CXA_MAX_SIZE];
static unsigned int __cxa_func_cnt = 0;

void *__dso_handle = 0;

void __cxa_pure_virtual()
{
    
}

unsigned int __cxa_find_free()
{
	if (__cxa_func_cnt<CXA_MAX_SIZE) return __cxa_func_cnt;

	for (unsigned int i=0; i<__cxa_func_cnt; i++) {
		if (__cxa_funcs[i].d==0) {
			return i;
		}
	}

	return CXA_MAX_SIZE+1;
}

int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso)
{
	unsigned int i = __cxa_find_free();
	if (i>=CXA_MAX_SIZE) return 1;

	__cxa_funcs[i].d = destructor;
	__cxa_funcs[i].arg = arg;
	__cxa_funcs[i].dso = dso;
	__cxa_func_cnt++;

	return 0;
}

void __cxa_free_all()
{
	for (unsigned int i=0; i<__cxa_func_cnt; i++) {
		if (__cxa_funcs[i].d!=0) {
			(*__cxa_funcs[i].d)(__cxa_funcs[i].arg);
		}
	}
}

void __cxa_free(void *f)
{
	for (unsigned int i=0; i<__cxa_func_cnt; i++) {
		if (f==__cxa_funcs[i].d) {
			(*__cxa_funcs[i].d)(__cxa_funcs[i].arg);
			__cxa_funcs[i].d = 0;
			break;
		}
	}
}

void __cxa_finalize(void *f)
{
	if (f==0) {
		__cxa_free_all();
	} else {
		__cxa_free(f);
	}
}
#endif

#ifdef ARCH_ARM
extern "C" int __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
	return __cxa_atexit(destroyer, object, dso_handle);
}
#endif
