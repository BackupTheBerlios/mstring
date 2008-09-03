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
 * eza/amd64/apic.c: implements local APIC support driver.
 *
 */

#include <eza/interrupt.h>
#include <eza/arch/8259.h>
#include <eza/arch/types.h>
#include <eza/arch/asm.h>
#include <eza/arch/apic.h>
#include <mlibc/kprintf.h>

/*
 * Black mages from intel and amd wrote that
 * local APIC is memory mapped, I'm afraid on this 
 * solution looks ugly ...
 * TODO: I get unclear sense while some higher 
 * abstraction not being implemented.
 */

static struct __local_apic_t *local_apic=(struct __local_apic_t *)APIC_BASE;

/* 
 * default functions to access APIC (local APIC)
 * I think that gcc can try to make optimization on it
 * to avoid I'm stay `volatile` flag here.
 */
static inline uint32_t __apic_read(ulong_t rv)
{
  return *((volatile uint32_t *)(APIC_BASE+rv));
}

static inline void __apic_write(ulong_t rv,uint32_t val)
{
  *((volatile uint32_t *)(APIC_BASE+rv))=val;
}

static uint32_t __get_maxlvt(void)
{
  return local_apic->version.max_lvt;
}

static void __set_lvt_lint_vector(uint32_t lint_num,uint32_t vector)
{
  if(!lint_num)
    local_apic->lvt_lint0.vector=vector;
  else
    local_apic->lvt_lint1.vector=vector;
}

static void __enable_apic(void)
{
  local_apic->svr.apic_enabled |= (1 << 0);
  local_apic->svr.cpu_focus |= (1 << 0);
}

static void __disable_apic(void)
{
  local_apic->svr.apic_enabled &= ~(1 << 0);
  local_apic->svr.cpu_focus &= ~(1 << 0);
}

void __local_apic_clear(void)
{
  uint32_t max_lvt;
  uint32_t v;

  max_lvt=__get_maxlvt();

  if(max_lvt>=3) {
    v=0xfe;
    local_apic->lvt_error.vector=v;
    local_apic->lvt_error.mask |= (1 << 0);
  }

  /* mask timer and LVTs*/
  local_apic->lvt_timer.mask |= (1 << 0);
  local_apic->lvt_lint0.mask |= (1 << 0);
  local_apic->lvt_lint1.mask |= (1 << 0);

  if(max_lvt>=4) 
    local_apic->lvt_pc.mask |= (1 << 0);
  
}

/*init functions makes me happy*/
void local_bsp_apic_init(void)
{
  uint32_t v;

  kprintf("{INFO} Initing APIC: \n");

  v=local_apic->version.version;
  kprintf("[LW] APIC version: %d\n",v);
  /* first we're need to clear APIC to avoid magical results */
  //  __local_apic_clear();

  /* enable APIC */
  __enable_apic();

  /* enable wire mode*/
  /* set nil vectors */

  __set_lvt_lint_vector(0,0x0);
  __set_lvt_lint_vector(1,0x0);

  /*set mode#7 for lint0*/
  local_apic->lvt_lint0.tx_mode=0x7;
  /*set mode#4 for lint1*/
  local_apic->lvt_lint1.tx_mode=0x4;

  /*tx_mode & polarity set to 0 on both lintx*/
  local_apic->lvt_lint0.tx_status |= (1 << 0);
  local_apic->lvt_lint0.polarity |= (1 << 0);
  local_apic->lvt_lint1.tx_status |= (1 << 0);
  local_apic->lvt_lint1.polarity |= (1 << 0);

}

#ifdef CONFIG_SMP

void arch_smp_init(void)
{
  uint32_t n_cpus=0;

}

#endif /* CONFIG_SMP */

