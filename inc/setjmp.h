#ifndef SETJMP_H
#define SETJMP_H

struct __jmp_buf
{
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
        unsigned eip, eflags;
};
typedef struct __jmp_buf jmp_buf[1];


#ifdef __cplusplus
extern "C" {
#endif
long setjmp(jmp_buf);
void longjmp(jmp_buf, long);
#ifdef __cplusplus
}
#endif

#endif
