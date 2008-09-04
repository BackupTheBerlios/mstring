
#include <eza/task.h>
#include <mm/pt.h>
#include <eza/smp.h>
#include <eza/kstack.h>
#include <eza/errno.h>
#include <mm/pagealloc.h>

#include <eza/kernel.h>
#include <mlibc/kprintf.h>
#include <eza/smp.h>
#include <eza/arch/scheduler.h>
#include <eza/arch/types.h>
#include <eza/arch/bits.h>
#include <eza/task.h>
#include <mm/pt.h>
#include <eza/scheduler.h>
#include <eza/swks.h>
#include <eza/kstack.h>
#include <mm/pagealloc.h>
#include <eza/arch/page.h>
#include <eza/pageaccs.h>


DEFINE_PER_CPU(pagelist_page_accessor_smp,page_frame_accessor_t);

int initialize_stack_system_area(kernel_task_data_t *t)
{
  uintptr_t task_vaddr = t->task.kernel_stack.low_address & KERNEL_STACK_MASK;
  page_idx_t tidx = virt_to_pframe_id(t);

  kprintf( "== Stack base: 0x%X, Page ID: 0x%X\n", task_vaddr, tidx );
//  mm_map_pages( &(t->task.page_dir));
//  kprintf( "== Vaddr of page 0x%X is 0x%X\n", pidx, vaddr );
//  page_idx_t pidx = mm_pin_virtual_address( &task->page_dir,
//                                            task->kernel_stack.low_address & KERNEL_STACK_MASK );
//  if( pidx != INVALID_PAGE_IDX ) {
//    void *vaddr = pframe_id_to_virt(pidx);

//    kprintf( "== Vaddr of page 0x%X is 0x%X\n", pidx, vaddr );

    return 0;
//  }
//  return -EFAULT;
}

int setup_task_kernel_stack(task_t *task)
{
  int r = allocate_kernel_stack(&task->kernel_stack);

  if( r == 0 ) {
    r = mm_map_pages( &task->page_dir, NULL,
                      task->kernel_stack.low_address, KERNEL_STACK_PAGES,
                      KERNEL_STACK_PAGE_FLAGS, NULL );
  }
  return r;
}

void initialize_task_system_data(kernel_task_data_t *task, cpu_id_t cpu)
{
  task->system_data.cpu_id = cpu;
  task->system_data.irq_num = 0;
}

status_t create_new_task()
{
  task_t *task;
  page_frame_t *ts_page;

  
}

