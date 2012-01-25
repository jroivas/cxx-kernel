#include "processmanager.h"
#include "arch/platform.h"
#include "kernel.h"
#include "mutex.h"

#define SCHEDULING_GRANULARITY (KERNEL_FREQUENCY>>1)
#define SCHEDULING_GRANULARITY_MIN (KERNEL_FREQUENCY/50)

static ptr_val_t __pm_mutex = 0;

ProcessManager::ProcessManager()
{
	for (uint32_t i=0; i<TASK_POOLS; i++) {
		m_task_pool[i] = new List();
	}
	m_tasks = new List();
	m_pool_index = 0;
	m_pool_target = 0;
	m_current = NULL;
	m_running = false;
	m_pid = 1;

	m.assign(&__pm_mutex);
}

uint32_t ProcessManager::approxPool(Task *t, uint32_t base)
{
	if (t==NULL) return TASK_POOLS - 1;

	uint32_t approx = (MAX_PRIORITY - t->priority()) + t->nice();
	approx = approx * TASK_POOLS / (MAX_PRIORITY + MAX_NICE + 1);
	(void)base;

	approx = (m_pool_index+approx+base)%TASK_POOLS; 
	return approx;
}

void ProcessManager::addTask(Task *t)
{
	if (t==NULL) return;

	//m.lock();
	Platform::seizeInterrupts();

	if (m_current!=NULL) m_current->lock();
	if (t->pid()==0) {
		t->setPid(m_pid++);
	}
	t->lock();
	//m_tasks->append(t);

#if 0
	if (m_current==NULL) {
		m_current = t;
	}
#endif
	//if (m_current!=NULL) m_current->unlock();
	
	volatile ptr_val_t *lock = NULL;
	if (m_current!=NULL) {
		lock = m_current->getLock();
		m_current->save();
		m_tasks->append(m_current);
	}

	//t->lock();
	m_current = t;

	t->unlock();
	Platform::continueInterrupts();

	t->switchTo(lock, (ptr_t)&ProcessManager::killer);

	//m.unlock();
}

void ProcessManager::doKill()
{
	Platform::video()->printf("Killing time!\n");
	if (m_current == NULL) return;

	m_tasks->deleteAll(m_current);
}

void ProcessManager::killer()
{
	ProcessManager *pm = Platform::processManager();
	pm->doKill();
}

Task *ProcessManager::schedule()
{
	Platform::seizeInterrupts();

	//Platform::video()->printf("Curr %x %d\n",m_current, m_current==NULL?0:m_current->pid());
	if (m_current!=NULL) {
		m_current->lock();
		m_tasks->append(m_current);
	}

	void *tmp = m_tasks->takeFirst();
	if (tmp==NULL) {
		Platform::continueInterrupts();
		return NULL;
	}

	Task *next = (Task*)tmp;
	next->lock();

	volatile ptr_val_t *lock = NULL;
	if (m_current!=NULL) {
		lock = m_current->getLock();
	}

	m_current->save();

	m_current = next;

#if 1
	uint32_t approx = (MAX_PRIORITY - m_current->priority()) + m_current->nice();
	if (approx==0) approx++;
	//m_pool_slice = SCHEDULING_GRANULARITY/approx;
	if (m_tasks->size()>0) {
		m_pool_slice = SCHEDULING_GRANULARITY/m_tasks->size()/approx;
		if (m_pool_slice<SCHEDULING_GRANULARITY_MIN) m_pool_slice = SCHEDULING_GRANULARITY_MIN;
	} else m_pool_slice = SCHEDULING_GRANULARITY/approx;

	next->setSlice(m_pool_slice);
#else
	next->setSlice(SCHEDULING_GRANULARITY);
#endif

#ifdef DEBUG
	Platform::video()->printf("\n::: %s slice: %5d,  %d\n",next->name(), m_pool_slice, m_tasks->size());
#endif
	next->unlock();
	Platform::continueInterrupts();

	next->restore(lock);

	return m_current;
}
