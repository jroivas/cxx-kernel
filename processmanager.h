#ifndef _PROCESS_MANAGER_H
#define _PROCESS_MANAGER_H

#include "task.h"
#include "list.h"

class ProcessManager
{
public:
	ProcessManager();
	void addTask(Task *t);
	void schedule();

private:
	List *m_tasks;
	Task *m_current;
};

#endif
