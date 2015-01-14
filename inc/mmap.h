#ifndef MMAP_H
#define MMAP_H

#ifdef __cplusplus
extern "C" {
#endif
void *mapPhys(void *phys, unsigned int length);
void unmapPhys(void *phys, unsigned int length);
#ifdef __cplusplus
}
#endif

#endif
