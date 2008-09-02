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
 * (c) Copyright 2008 Tirra <tirra.newly@gmail.com>
 *
 * eza/amd64/cpu.c: varios CPU functions
 *
 */

#include <eza/arch/types.h>
#include <eza/arch/cpu.h>
#include <eza/arch/asm.h>

extern void syscall_point(void);

/* cpu_setup_fpu(): checkin CPU flags 
 * to make FPU usable for use
 */
void cpu_setup_fpu(void)
{
  asm volatile(
	       "movq %%cr0, %%rax;"
	       "btsq $1, %%rax;"
	       "btrq $2, %%rax;"
	       "movq %%rax, %%cr0;"
	       "movq %%cr4, %%rax;"
	       "bts $9, %%rax;"
	       "movq %%rax, %%cr4;"
	       :
	       :
	       :"%rax");

}

/* init syscall/sysret entry function */
void arch_syscall_setup_cpu(void)
{
  /* enable */
  set_efer_flag(AMD_SCE_FLAG);
  /* setup the entry address */
  write_msr(AMD_MSR_STAR,((uint64_t)(gdtselector(KDATA_DES) | PL_USER) << 48) | 
	    ((uint64_t)(gdtselector(KTEXT_DES) | PL_KERNEL) << 32));
  write_msr(AMD_MSR_LSTAR,(uint64_t)syscall_point);
  write_msr(AMD_MSR_SFMASK,0x200);

  return;
}
