#include "processmanager.h"
#include "arch/platform.h"

ProcessManager::ProcessManager()
{
	m_tasks = new List();
	m_current = NULL;
}

void ProcessManager::addTask(Task *t)
{
	if (t==NULL) return;
	m_tasks->append(t);

	if (m_current==NULL) {
		m_current = t;
	}
}

void ProcessManager::schedule()
{
	// This is crucial
	Platform::seizeInterrupts();

	ptr_val_t inst = m_current->saveState();

	if (inst==TASK_MAGIC) {
		return;
	}

	m_tasks->append(m_current);

	void *tmp = m_tasks->takeFirst();
	if (tmp==NULL) {
		return;
	}

	m_current = (Task*)tmp;
	m_current->switchTo();
}
