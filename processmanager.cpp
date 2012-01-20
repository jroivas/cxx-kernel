#include "processmanager.h"

ProcessManager::ProcessManager()
{
	m_tasks = new List();
}

void ProcessManager::addTask(Task *t)
{
	if (t==NULL) return;
	m_tasks->append(t);
}
