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
 * (c) Copyright 2005,2008 Tirra <tirra.newly@gmail.com>
 * (C) Copyright 2008 Michael Tsymbalyuk <mtzaurus@gmail.com>
 *
 * /eza/amd64/asm.S: low level assembler functions specific for amd64
 *                   architecture
 *
 */

#include <eza/arch/context.h>

.code64

.text
.global set_efer_flag
.global syscall_point

.global arch_delay_loop
.global arch_fake_loop
	
.global divide_by_zero_fault_handler
.global debug_fault_handler
.global nmi_fault_handler
.global breakpoint_fault_handler
.global overflow_fault_handler
.global bound_range_fault_handler
.global invalid_opcode_fault_handler
.global device_not_available_fault_handler
.global doublefault_fault_handler
.global invalid_tss_fault_handler
.global segment_not_present_fault_handler
.global stack_fault_handler
.global general_protection_fault_handler
.global page_fault_fault_handler
.global fpu_fault_handler
.global alignment_check_fault_handler
.global machine_check_fault_handler
.global simd_fault_handler
.global security_exception_fault_handler
.global hi_level_fault_handlers 

.global child_fork_path

arch_delay_loop:
0:	dec %rdi
	jnz 0b
	ret

arch_fake_loop:
0:	dec %rdi
	jz 0b
	ret
	
set_efer_flag:
	movq $0xc0000080, %rcx
	rdmsr
	btsl %edi, %eax
	wrmsr
	ret

	/* low-level syscall binding, registers used:	
	 * rcx - uspace address to out, rax - syscall number,
	 * rdi, rsi, rdx, r10, r8, r9 - arguments (from 1 - 6), rax returning value
	 */
syscall_point:
	swapgs
	movq %rsp, %gs:0
        movq %gs:8, %rsp
        swapgs
        sti
        
        pushq %rcx
        pushq %r11
        movq %r10, %rcx
        pushq %rax
	/* call real handler */
        call syscall_handler
        addq $8, %rsp
        popq %r11
        popq %rcx
        cli
        swapgs
        movq %gs:0, %rsp
        swapgs
	
        sysretq

child_fork_path:
    RESTORE_ALL
    iretq

/* Initialize low-level IRQ stubs. Thank Linux team for a very good approach !
 */
.global irq_entrypoints_array 
.global low_irq_stubs 

.data
irq_entrypoints_array:
.text

v=0
low_irq_stubs:

.rept 16
1:  pushq %rax
    movq $v,%rax
    jmp common_interrupt

.data
    .quad 1b

.text
v=v+1
.endr

common_interrupt:
  SAVE_ALL
  movq %rax,%rdi
  call do_irq
  RESTORE_ALL
  popq %rax
  iretq

/* Here goes handlers for CPU-specific fault handlers ranged at [0 .. 31]
 */
divide_by_zero_fault_handler:
  pushq %rax
  movl $0x0,%eax
common_exception_path:
  SAVE_ALL
  
  /* No we have to prepare interrupt/exception stack frame.
   * Main job willbe done in 'SAVE_ALL', bu we must add 8 bytes to
   * skip %rax on the stack since we saved it before calling SAVE_ALL
   * %r10 contains the value of %rsp before aligning for saving XMM data.
   */
  mov %r10, %rdi; \
  add $SAVED_GPR_SIZE + 8, %rdi 

  /* Now we call call the handler. */
  call *hi_level_fault_handlers(,%rax,8)
  RESTORE_ALL
  popq %rax
  iretq

debug_fault_handler:
  push %rax
  movl $0x1,%eax
  jmp common_exception_path

nmi_fault_handler:
  push %rax
  movl $0x2,%eax
  jmp common_exception_path

breakpoint_fault_handler:
  push %rax
  movl $0x3,%eax
  jmp common_exception_path

overflow_fault_handler:
  push %rax
  movl $0x4,%eax
  jmp common_exception_path

bound_range_fault_handler:
  push %rax
  movl $0x5,%eax
  jmp common_exception_path

invalid_opcode_fault_handler:
  push %rax
  movl $0x6,%eax
  jmp common_exception_path

device_not_available_fault_handler:
  push %rax
  movl $0x7,%eax
  jmp common_exception_path

doublefault_fault_handler:
  push %rax
  movl $0x8,%eax
  jmp common_exception_path

invalid_tss_fault_handler:
  push %rax
  movl $10,%eax
  jmp common_exception_path

segment_not_present_fault_handler:
  push %rax
  movl $11,%eax
  jmp common_exception_path

stack_fault_handler:
  push %rax
  movl $12,%eax
  jmp common_exception_path

general_protection_fault_handler:
  push %rax
  movl $13,%eax
  jmp common_exception_path

page_fault_fault_handler:
  push %rax
  movl $14,%eax
  jmp common_exception_path

fpu_fault_handler:
  push %rax
  movl $16,%eax
  jmp common_exception_path

alignment_check_fault_handler:
  push %rax
  movl $17,%eax
  jmp common_exception_path

machine_check_fault_handler:
  push %rax
  movl $18,%eax
  jmp common_exception_path

simd_fault_handler:
  push %rax
  movl $19,%eax
  jmp common_exception_path

security_exception_fault_handler:
  push %rax
  movl $30,%eax
  jmp common_exception_path 


/* Table that represents hi-level exception routines written in C.
 *            DON'T CHANGE THE ORDER IN THIS TABLE !
 */

hi_level_fault_handlers:
.quad divide_by_zero_fault_handler_impl        /* 0 */
.quad debug_fault_handler_impl                 /* 1 */
.quad nmi_fault_handler_impl                   /* 2 */
.quad breakpoint_fault_handler_impl            /* 3 */
.quad overflow_fault_handler_impl              /* 4 */
.quad bound_range_fault_handler_impl           /* 5 */
.quad invalid_opcode_fault_handler_impl        /* 6 */
.quad device_not_available_fault_handler_impl  /* 7 */

.quad doublefault_fault_handler_impl           /* 8 */
.quad reserved_fault_handler_impl              /* 9 */
.quad invalid_tss_fault_handler_impl           /* 10 */
.quad segment_not_present_fault_handler_impl   /* 11 */
.quad stack_fault_handler_impl                 /* 12 */
.quad general_protection_fault_handler_impl    /* 13 */
.quad page_fault_fault_handler_impl            /* 14 */
.quad reserved_fault_handler_impl              /* 15 */

.quad fpu_fault_handler_impl                   /* 16 */
.quad alignment_check_fault_handler_impl       /* 17 */
.quad machine_check_fault_handler_impl         /* 18 */
.quad simd_fault_handler_impl                  /* 19 */
.quad reserved_fault_handler_impl              /* 20 */
.quad reserved_fault_handler_impl              /* 21 */
.quad reserved_fault_handler_impl              /* 22 */
.quad reserved_fault_handler_impl              /* 23 */

.quad reserved_fault_handler_impl              /* 24 */
.quad reserved_fault_handler_impl              /* 25 */
.quad reserved_fault_handler_impl              /* 26 */
.quad reserved_fault_handler_impl              /* 27 */
.quad reserved_fault_handler_impl              /* 28 */
.quad reserved_fault_handler_impl              /* 29 */
.quad security_exception_fault_handler_impl    /* 30 */
.quad reserved_fault_handler_impl              /* 31 */


