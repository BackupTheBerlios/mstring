#include <eza/task.h>
#include <mm/pt.h>
#include <eza/smp.h>
#include <eza/kstack.h>
#include <eza/errno.h>
#include <mm/pagealloc.h>
#include <eza/amd64/context.h>
#include <mlibc/kprintf.h>
#include <eza/arch/scheduler.h>
#include <eza/arch/types.h>
#include <eza/kernel.h>
#include <eza/pageaccs.h>
#include <eza/list.h>

extern task_t *kthread1;

status_t do_fork(void *arch_ctx, task_creation_flags_t flags)
{
  task_t *new_task;
  status_t r;
  task_t *parent = current_task();

  r = create_new_task(parent,&new_task,flags);
  if(r == 0) {
    r = arch_copy_process(parent,new_task,arch_ctx,flags);
    if(r == 0) {
      /* New task is ready. */
      kthread1 = new_task;
    } else {
    }
  }

  return r;
}

