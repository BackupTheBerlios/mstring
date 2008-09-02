
#ifndef __TASK_H__
#define __TASK_H__ 

#include <eza/scheduler.h>
#include <eza/kstack.h>

int setup_task_kernel_stack(task_t *task);
int initialize_stack_system_area(kernel_task_data_t *task);
void initialize_task_system_data(kernel_task_data_t *task, cpu_id_t cpu);

#endif

