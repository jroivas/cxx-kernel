#include "task.h"

Task::Task()
{
    m_name[0] = 'n';
    m_name[1] = 'a';
    m_name[2] = 'd';
    m_name[3] = 'a';
    m_name[4] = 0;
    m_stack = 0;

    m_priority = 1;
    m_nice = 1;
    m_slice = 0;
    m_size = 0;

    m_pid = 0;
    m_status = STATUS_WAITING;

    m_userSpace = false;
    wait.task = this;
}
