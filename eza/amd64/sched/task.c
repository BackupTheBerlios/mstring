/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * (c) Copyright 2006,2007,2008 MString Core Team <http://mstring.berlios.de>
 * (c) Copyright 2008 Michael Tsymbalyuk <mtzaurus@gmail.com>
 *
 * eza/amd64/sched/task.c: AMD64-specific tasks-related functions.
 */

#include <eza/task.h>
#include <eza/arch/context.h>
#include <mlibc/string.h>
#include <eza/arch/page.h>
#include <mlibc/kprintf.h>
#include <mm/mm.h>
#include <eza/errno.h>
#include <mlibc/string.h>
#include <eza/smp.h>
#include <eza/pageaccs.h>
#include <mm/pagealloc.h>
#include <eza/kernel.h>

/* Located on 'amd64/asm.S' */
extern void child_fork_path(void);

status_t arch_setup_task_context(task_t *newtask,task_creation_flags_t flags)
{
  arch_context_t *ctx = (arch_context_t*)&(newtask->arch_context[0]);

  /* Setup CR3 and RSP for the new task. */
  ctx->rsp = 0;
  ctx->cr3 = _k2p((uintptr_t)&(newtask->page_dir.entries[0]));

  return 0;
}

status_t arch_copy_process(task_t *parent,task_t *newtask,void *arch_ctx,
                           task_creation_flags_t flags)
{
  uint64_t *p = (uint64_t *)arch_ctx;
  uint64_t ip,offset;
  status_t r = -EINVAL;
  regs_t *regs;

  /* See 'SAVE_ALL' for detailed context layout. */
  ip = *p++;
  /* Start address must belong to the kernel code. */
  if(ip >= KERNEL_BASE && ip < (uint64_t)KERNEL_FIRST_FREE_ADDRESS) {
    offset = *p + sizeof(regs_t);

    if(offset < PAGE_SIZE && (offset & 0x1ff) == 0 ) {
      char *stack = (char *)newtask->kernel_stack.high_address - offset;
      arch_context_t *ctx = (arch_context_t*)&newtask->arch_context[0];

      regs = (regs_t *)newtask->kernel_stack.high_address - 1;

      /* Adjust 'offset value' and 'start address' since they must also
       * be on the stack.
       */
      stack -= 16;
      offset += 16;

      /* Copy context to new task's kernel stack. */
      memcpy(stack,arch_ctx,offset);

      regs->old_rsp = newtask->kernel_stack.high_address - 128;
      /* cr3 was already setup in 'arch_setup_task_context()' */
      ctx->rsp = (uint64_t)stack;

      r = 0;
    } else {
      kprintf( KO_WARNING "arch_copy_process(): Insufficient context offset: %d\n",
               offset );
    }
  } else {
    kprintf( KO_WARNING "arch_copy_process(): Insufficient execution address: 0x%X\n",
             ip );
  }

  kprintf( "r = %d\n", r );

  return r;
}

void kernel_thread_helper(void (*fn)(void*), void *data)
{
  kprintf( "** NEW KERNEL THREAD IS STARTING !!! DATA: %s **\n", data );
  fn(data);
  l2: goto l2;
}

status_t kernel_thread(void (*fn)(void *), void *data)
{
  char stack[1256];
  regs_t *regs = (regs_t *)((uint64_t)&(stack[1256]) - sizeof(regs_t));
  char *fsave;
  uint64_t flags, delta = 0;
  uint64_t t1, t2;

  /* 0x1000 means 'any page-aligned address'. */
  t1 = (0x1000 - sizeof(regs_t));
  t2 = t1 & 0xe00;
  /* Calculate offset to the nearest 512-bytes boundary. */
  delta = (uint64_t)t1 - (uint64_t)t2;
  /* After this we will be 100% able to store 512-bytes XMM context. */
  delta += 512;

  /* Prepare a fake CPU-saved context */
  memset( regs, 0, sizeof(regs_t) );

  /* Save stack. */
  regs->old_ss = gdtselector(KDATA_DES);
  regs->old_rsp = 0; /* Will be initialized later. */

  /* Save flags. */
  __asm__ volatile (
    "pushfq\n"
    "popq %0\n"
    : "=r" (flags) );

  regs->rflags = flags | 0x200; /* Enable interrupts. */
  regs->cs = gdtselector(KTEXT_DES);
  regs->rip = (uint64_t)kernel_thread_helper;

  /* Prepare entrypoint. */
  regs->rdi = (uint64_t)fn;
  regs->rsi = (uint64_t)data;

  /* Now prepare XMM context. */
  fsave = (char *)regs - delta;
  memset( fsave, 0, 512 );

  /* Save size of this context for further use in RESTORE_ALL. */
  fsave -= 8;
  *((uint64_t *)fsave) = delta;

  /* Now save the return point on the stack. */
  fsave -= 8;
  *((uint64_t *)fsave) = (uint64_t)child_fork_path;

  /* The context is ready, so just 'do_fork()' ! */
  return do_fork(fsave, CLONE_MM);
}


/* For initial stack filling. */
static page_frame_t *next_frame;
static page_idx_t acc_next_frame(void *ctx)
{
  if(next_frame != NULL ) {
    return next_frame->idx;
  } else {
    page_frame_t *frame = alloc_page(0,0);
    if( frame == NULL ) {
      panic( "initialize_idle_tasks(): Can't allocate a page !" );
    }

    return frame->idx;
  }
}

static page_frame_accessor_t idle_pacc = {
  .frames_left = pageaccs_frames_left_stub,
  .next_frame = acc_next_frame,
  .reset = pageaccs_reset_stub,
  .alloc_page = pageaccs_alloc_page_stub,
};


void initialize_idle_tasks(void)
{
  task_t *task;
  page_frame_t *ts_page;
  int r, cpu;
  kernel_task_data_t *td;

  for( cpu = 0; cpu < NR_CPUS; cpu++ ) {
    ts_page = alloc_page(0,1);
    if( ts_page == NULL ) {
      panic( "initialize_idle_tasks(): Can't allocate main structure for idle task !" );  
    }

    td = (kernel_task_data_t *)pframe_to_virt(ts_page);
    task = &(td->task);
    idle_tasks[cpu] = task;

    /* Setup PIDs and default priorities. */
    task->pid = task->ppid = 0;
    task->priority = task->static_priority = IDLE_TASK_PRIORITY;
    task->time_slice = 0;
    task->state = TASK_STATE_RUNNING;

    /* Initialize page tables to default kernel page directory. */
    initialize_page_directory(&task->page_dir);
    task->page_dir.entries = kernel_pt_directory.entries;

    /* Initialize kernel stack.
     * Since kernel stacks aren't properly initialized, we can't use standard
     * API that relies on 'cpu_id'.
     */
    if( allocate_kernel_stack(&task->kernel_stack) != 0 ) {
      panic( "initialize_idle_tasks(): Can't initialize kernel stack for idle task !" ); 
    }

    next_frame = NULL;
    r = mm_map_pages( &task->page_dir, &idle_pacc,
                      task->kernel_stack.low_address, KERNEL_STACK_PAGES,
                      KERNEL_STACK_PAGE_FLAGS, NULL );
    if( r != 0 ) {
      panic( "initialize_idle_tasks(): Can't map kernel stack for idle task !" );
    }

    /* OK, stack is already mapped, so we can map task struct into this task's address
     * space.
     */
    next_frame = ts_page;
    r = mm_map_pages( &task->page_dir, &idle_pacc,
                      task->kernel_stack.low_address & KERNEL_STACK_MASK, 1,
                      KERNEL_STACK_PAGE_FLAGS, NULL );
    if( r != 0 ) {
      panic( "initialize_idle_tasks(): Can't map kernel stack for idle task !" );
    }

    /* Setup arch-specific task context. */
    r = arch_setup_task_context(task,0);
    if( r != 0 ) {
      panic( "initialize_idle_tasks(): Can't setup arch-specific context for idle task !" );
    }

    /* OK, now kernel stack is ready for this idle task. Finally, initialize its
     * 'system_data' structure.
     */
    initialize_task_system_data(td, cpu);
  }
}

