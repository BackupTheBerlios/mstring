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
 * eza/amd64/fault.c: contains routines for dealing with x86_64 CPU fauls.
 *
 */

#include <mm/page.h>
#include <eza/arch/types.h>
#include <eza/arch/page.h>
#include <eza/arch/fault.h>
#include <eza/arch/mm_types.h>
#include <eza/arch/interrupt.h>
#include <eza/kernel.h>
#include <mlibc/kprintf.h>

#define kernel_fault(f) \
    (f->cs == gdtselector(KTEXT_DES))

typedef struct __fault_descr {
  uint32_t slot;
  void (*handler)();
} fault_descr_t;

/* The list of exceptions we want to install. */
static fault_descr_t faults_to_install[] = {
  {DE_FAULT, divide_by_zero_fault_handler},
  {DB_FAULT, debug_fault_handler},
  {NMI_FAULT, nmi_fault_handler},
  {BP_FAULT, breakpoint_fault_handler},
  {OF_FAULT, overflow_fault_handler},
  {BR_FAULT, bound_range_fault_handler},
  {UD_FAULT, invalid_opcode_fault_handler},
  {NM_FAULT, device_not_available_fault_handler},
  {DF_FAULT, doublefault_fault_handler},
  {TS_FAULT, invalid_tss_fault_handler},
  {NP_FAULT, segment_not_present_fault_handler},
  {SS_FAULT, stack_fault_handler},
  {GP_FAULT, general_protection_fault_handler},
  {PF_FAULT, page_fault_fault_handler},
  {MF_FAULT, fpu_fault_handler},
  {AC_FAULT, alignment_check_fault_handler},
  {MC_FAULT, machine_check_fault_handler},
  {XF_FAULT, simd_fault_handler},
  {SX_FAULT, security_exception_fault_handler},
  {0,0},
};


void divide_by_zero_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
  kprintf( "  [!!] #DE exception raised !\n" );
}

void debug_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void nmi_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void breakpoint_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void overflow_fault_handler_impl(void)
{
}

void bound_range_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void invalid_opcode_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void device_not_available_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void doublefault_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
}

void coprocessor_segment_overrun_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void invalid_tss_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
}

void segment_not_present_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
}

void stack_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
}

void general_protection_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
  if( kernel_fault(stack_frame) ) {
    kprintf( "#GPF in kernel mode: RIP = 0x%X\n", stack_frame->rip );    
  }
  kprintf( "[!!!] Unhandled GPF exception ! Stopping ...\n" );
  l1: goto l1;
}

void page_fault_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
  if( kernel_fault(stack_frame) ) {
    kprintf( "#PF in kernel mode: RIP = 0x%X\n", stack_frame->rip );    
  }
  kprintf( "[!!!] Unhandled PF exception ! Stopping ...\n" );
  l1: goto l1;

}

void reserved_exception_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void fpu_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void alignment_check_fault_handler_impl(interrupt_stack_frame_err_t *stack_frame)
{
}

void machine_check_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void simd_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void security_exception_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}

void reserved_fault_handler_impl(interrupt_stack_frame_t *stack_frame)
{
}


void install_fault_handlers(void)
{
  int idx, r;

  /* Install all known fault handlers. */
  for( idx = 0; faults_to_install[idx].handler != NULL; idx++ ) {
    r = install_trap_gate(faults_to_install[idx].slot,
                          (uintptr_t)faults_to_install[idx].handler,PROT_RING_0,0);
    if(r != 0) {
      panic( "Can't install fault handler #%d", faults_to_install[idx].slot );
    }
  }
}

