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
	bool isRunning() { return m_running; }
	void setRunning() { m_running=true; }
	bool hasSlice() { if (m_current==NULL) return false; if (m_current->getSlice()>0) return true; return false; }
	void decSlice() { if (m_current!=NULL) m_current->decSlice(); }

private:
	bool m_running;
	List *m_tasks;
	Task *m_current;
};

#endif
