#ifndef CXA_H
#define CXA_H

#ifdef __cplusplus
extern "C"  {
#endif

#ifndef ARCH_LINUX
void __cxa_pure_virtual();
#if 0
int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);
void __cxa_finalize(void *f);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
