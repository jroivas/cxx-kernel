#ifndef SETJMP_H
#define SETJMP_H

#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
        unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
        unsigned eip, eflags;
} jmp_buf[1];

long setjmp(jmp_buf);
void longjmp(jmp_buf, long);
#ifdef __cplusplus
}
#endif

#endif
