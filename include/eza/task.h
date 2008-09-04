
#ifndef __TASK_H__
#define __TASK_H__ 

#include <eza/arch/types.h>
#include <eza/scheduler.h>
#include <eza/kstack.h>

int setup_task_kernel_stack(task_t *task);
int initialize_stack_system_area(kernel_task_data_t *task);
void initialize_task_system_data(kernel_task_data_t *task, cpu_id_t cpu);

pid_t kernel_thread(void (*fn)(void *), void *data);
pid_t do_fork();

#endif

