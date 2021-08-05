#include "regs.h"
#include "videox86.h"

void Regs::dump()
{
    VideoX86 tmp;

    tmp.printf("Regs:\n");
    tmp.printf(" GS: 0x%8x  FS: 0x%8x  ES: 0x%8x  DS: 0x%8x\n", gs, fs, es, ds);
    tmp.printf("EDI: 0x%8x ESI: 0x%8x EBP: 0x%8x ESP: 0x%8x\n", edi, esi, ebp, esp);
    tmp.printf("EAX: 0x%8x EBX: 0x%8x ECX: 0x%8x EDX: 0x%8x\n", eax, ebx, ecx, edx);
    tmp.printf("EIP: 0x%8x  CS: 0x%8x FLG: 0x%8x  SS: 0x%8x\n", eip, cs, eflags, ss);
    tmp.printf("INT: 0x%8x ERR: 0x%8x\n", int_no, err_code);
}
