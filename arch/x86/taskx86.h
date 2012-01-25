#ifndef _TASK_X86_H
#define _TASK_X86_H

#include "task.h"
#include "regs.h"
#include "string.h"

typedef struct
{
        unsigned short link;
        unsigned short r_link;
        unsigned long esp0;
        unsigned short ss0;
        unsigned short r_ss0;
        unsigned long esp1;
        unsigned short ss1;
        unsigned short r_ss1;
        unsigned long esp2;
        unsigned short ss2;
        unsigned short r_ss2;
        unsigned long cr3;
        unsigned long eip;
        unsigned long eflags;
        unsigned long eax;
        unsigned long ecx;
        unsigned long edx;
        unsigned long ebx;
        unsigned long esp;
        unsigned long ebp;
        unsigned long esi;
        unsigned long edi;
        unsigned short es;
        unsigned short r_es;
        unsigned short cs;
        unsigned short r_cs;
        unsigned short ss;
        unsigned short r_ss;
        unsigned short ds;
        unsigned short r_ds;
        unsigned short fs;
        unsigned short r_fs;
        unsigned short gs;
        unsigned short r_gs;
        unsigned short ldtr;
        unsigned short r_ldtr;
        unsigned short r_iopb;
        unsigned short iopb;
} task_tss_t;

class TaskX86 : public Task
{
public:
	TaskX86();
	void init(ptr_val_t addr, ptr_val_t stack, uint32_t flags);

	void switchTo(volatile ptr_val_t *lock, ptr_t killer);
	void save();
	void restore(volatile ptr_val_t *lock);

	Task *clone(CloneFlags flags = CLONE_NORMAL);
	Task *create(ptr_val_t addr, ptr_val_t stack, uint32_t flags);
	void setTss(void *ptr) { Mem::copy((void*)&m_tss, ptr, sizeof(task_tss_t)); }
	void setEntry(ptr_val_t addr);

private:
	task_tss_t m_tss;
	class StateX86 {
	public:
		uint32_t states[16];
	};
	StateX86 m_state;
};

#endif
