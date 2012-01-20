#ifndef TASK_H
#define TASK_H

#include "types.h"
#define TASK_MAGIC 0x42

class Task
{
public:
	enum CloneFlags {
		CLONE_NORMAL        = 0,
		CLONE_SHARE_ADDRESS = (1<<0),
		CLONE_SHARE_STACK   = (1<<1),
		CLONE_SHARE_FS      = (1<<2),
		CLONE_SHARE_FILES   = (1<<3),
		CLONE_SHARE_SIGNAL  = (1<<4),
		CLONE_SHARE_PARENT  = (1<<5)
	};

	Task();
	virtual void init(ptr_val_t addr, ptr_val_t stack, uint32_t flags) = 0;
	virtual void switchTo() = 0;
	virtual ptr_val_t saveState() = 0;
	virtual Task *clone(CloneFlags flags = CLONE_NORMAL) = 0;

protected:
	ptr_val_t m_stack;
	char m_name[32];
};

#endif
