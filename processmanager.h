#ifndef _PROCESS_MANAGER_H
#define _PROCESS_MANAGER_H

#include "task.h"
#include "list.h"

class ProcessManager
{
public:
	ProcessManager();
	void addTask(Task *t);

private:
	List *m_tasks;
};

#endif
