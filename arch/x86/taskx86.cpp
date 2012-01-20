#include "taskx86.h"
#include "string.h"
#include "mm.h"

#define DEFAULT_STACK_SIZE 0x1000

extern "C" void changeProcess(uint32_t eip, uint32_t base, uint32_t stack, uint32_t pagetable);
extern "C" uint32_t getEIP();
extern "C" uint32_t getESP();
extern "C" uint32_t getEBP();

void TaskX86::init(ptr_val_t addr, ptr_val_t stack, uint32_t flags)
{
	if (stack==0) {
		stack = (ptr_val_t)MM::instance()->alloc(DEFAULT_STACK_SIZE);
		if (stack==0) return;
	}
	m_stack = stack;
	r.eip = addr;
	r.esp = stack;
	r.useresp = stack;
	r.eflags = flags;

	//FIXME
	r.cs = 0x08;
	r.ds = 0x10;
	r.es = 0x10;
	r.ss = 0x10;
	r.fs = 0x10;
	r.gs = 0x10;
}

/* Save task state and return instruction pointer */
ptr_val_t TaskX86::saveState()
{
	r.esp = getESP();
	r.ebp = getEBP();
	ptr_val_t eip = getEIP();
	if (eip!=TASK_MAGIC) {
		r.eip = eip;
	}

	return eip;
}

void TaskX86::switchTo()
{
	changeProcess(r.eip, r.ebp, r.esp, 0);
}

Task *TaskX86::clone(TaskX86::CloneFlags flags)
{
	TaskX86 *t = new TaskX86();

	ptr_val_t esp = 0;
	if ((flags & CLONE_SHARE_STACK)>0) esp = r.esp;

	t->init(r.eip, esp, r.eflags);

	return t;
}
