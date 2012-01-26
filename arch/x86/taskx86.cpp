#include "taskx86.h"
#include "string.h"
#include "mm.h"
#include "gdt.h"
#include "../platform.h"

#define DEFAULT_STACK_SIZE 0x1000

extern "C" int saveProcess(uint32_t *stack);
extern "C" int restoreProcess(volatile uint32_t *lock, uint32_t *stack);
extern "C" void changeProcess(volatile uint32_t *lock, uint32_t eip, uint32_t stack, uint32_t parm, uint32_t *killer);
extern "C" uint32_t getEIP();
extern "C" uint32_t getEIP2();
extern "C" uint32_t getESP();
extern "C" uint32_t getEBP();
extern "C" uint32_t getCR3();

TaskX86::TaskX86() : Task()
{
	setTss(getTss0());
	Mem::set(&m_state, 0, sizeof(StateX86));
}

void TaskX86::setEntry(ptr_val_t addr)
{
	m_tss.eip = addr;
}

void TaskX86::init(ptr_val_t addr, ptr_val_t stack, uint32_t flags)
{

	if (stack==0) {
		m_stack = (ptr_val_t)MM::instance()->alloc(DEFAULT_STACK_SIZE);
		if (m_stack!=0) m_stack += DEFAULT_STACK_SIZE-4; //Stack grows downward
	} else {
		m_stack = stack;
	}

	m_stack_syscall = (ptr_val_t)MM::instance()->alloc(DEFAULT_STACK_SIZE);
	m_stack_syscall += DEFAULT_STACK_SIZE-4;

	Mem::set(&m_tss, 0, sizeof(task_tss_t));

	//FIXME, set user space/kernel space accordingly
        m_tss.cs = (unsigned short)0x08;
        m_tss.ds = (unsigned short)0x10;
        m_tss.cs = (unsigned short)0x10;
        m_tss.es = (unsigned short)0x10;
        m_tss.gs = (unsigned short)0x10;
        m_tss.ss0 = (unsigned short)0x10;

        m_tss.eip = addr;
        m_tss.eflags = flags;
	m_entry = addr;

        m_tss.esp = m_stack;
        m_tss.cr3 = getCR3(); //FIXME

        m_tss.esp0 = m_stack_syscall;
        m_tss.iopb = (unsigned short)sizeof(task_tss_t) - 1;
}

void TaskX86::switchTo(volatile ptr_val_t *lock, ptr_t killer)
{
	gdt_set_gate(5, (unsigned long) &m_tss, sizeof(task_tss_t) - 1, 0x89, 0x40);
	changeProcess((volatile uint32_t*)lock, m_tss.eip, m_tss.esp, 0, (uint32_t*)killer);
}

int TaskX86::save()
{
	return saveProcess((uint32_t*)&m_state);
}

int TaskX86::restore(volatile ptr_val_t *lock)
{
	//Platform::video()->printf("Stack: %x %x, diff: %d\n", m_state.states[4], m_stack, m_stack-m_state.states[4]);
	//Platform::video()->printf("Stack: %x %x, diff: %d\n", m_tss.esp0, m_stack_syscall, m_stack_syscall-m_tss.esp0);
	gdt_set_gate(5, (unsigned long) &m_tss, sizeof(task_tss_t) - 1, 0x89, 0x40);
	return restoreProcess((volatile uint32_t*)lock, (uint32_t*)&m_state);
}

Task *TaskX86::clone(TaskX86::CloneFlags flags)
{
	TaskX86 *t = new TaskX86();
	if (t==NULL) return NULL;
	//FIXME

	return t;
}

Task *TaskX86::create(ptr_val_t addr, ptr_val_t stack, uint32_t flags)
{
	TaskX86 *t = new TaskX86();
	if (t==NULL) return NULL;

	t->init(addr, stack, flags);
	return t;
}
