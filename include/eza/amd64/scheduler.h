#ifndef __ARCH_SCHEDULER_H__
#define __ARCH_SCHEDULER_H__ 

#include <eza/arch/types.h>
#include <eza/arch/page.h>
#include <eza/scheduler.h>
#include <eza/kstack.h>

static inline task_t *arch_current_task(void)
{
  kernel_task_data_t *t;

  __asm__ volatile( "movq %%rsp, %%rbx\n"
                    "andq %%rax, %%rbx\n"
                     : "=b" (t) : "a" (KERNEL_STACK_MASK) );
  return &t->task;
}

static inline system_sched_data_t *arch_system_sched_data(void)
{
  kernel_task_data_t *t;

  __asm__ volatile( "movq %%rsp, %%rbx\n"
                    "andq %%rax, %%rbx\n"
                     : "=b" (t) : "a" (KERNEL_STACK_MASK) );
  return &t->system_data;
}

#endif

