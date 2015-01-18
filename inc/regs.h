#ifndef REGS_H
#define REGS_H

class Regs
{
public:
// FIXME
#ifdef ARCH_x86
    Regs() {
        gs=0;fs=0;es=0;ds=0;
        edi=0;esi=0;ebp=0;esp=0;ebx=0;edx=0;ecx=0;edx=0;
        int_no=0;err_code=0;
        eip=0;cs=0;eflags=0;useresp=0;ss=0;
    }
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
#endif
};

#endif
