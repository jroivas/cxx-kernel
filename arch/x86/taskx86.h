#ifndef _TASK_X86_H
#define _TASK_X86_H

#include "task.h"
#include "regs.h"

class TaskX86 : public Task
{
public:
	TaskX86() : Task() { }
	void init(ptr_val_t addr, ptr_val_t stack, uint32_t flags);
	void switchTo();
	void saveState();
	Task *clone(CloneFlags flags = CLONE_NORMAL);

private:
	Regs r;
};

#endif
