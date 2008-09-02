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
 * include/eza/amd64/context.h: structure definion for context and related stuff
 *                              assembler macros and some constants
 *
 */

#ifndef __ARCH_CONTEXT_H__
#define __ARCH_CONTEXT_H__ /* there are several context.h(es) */

#define OFFSET_SP   0x0
#define OFFSET_PC   0x8
#define OFFSET_RBX  0x10
#define OFFSET_RBP  0x18
#define OFFSET_R12  0x20
#define OFFSET_R13  0x28
#define OFFSET_R14  0x30
#define OFFSET_R15  0x38
#define OFFSET_IPL  0x40

#define OFFSET_TLS  OFFSET_IPL

/* assembler macros for save and restore context */
#ifdef __ASM__

.macro CONTEXT_SAVE_ARCH_CORE  xc:req pc:req
  movq \pc, OFFSET_PC(\xc)
  movq %rsp, OFFSET_SP(\xc)
  movq %rbx, OFFSET_RBX(\xc)
  movq %rbp, OFFSET_RBP(\xc)
  movq %r12, OFFSET_R12(\xc)
  movq %r13, OFFSET_R13(\xc)
  movq %r14, OFFSET_R14(\xc)
  movq %r15, OFFSET_R15(\xc)
.endm

.macro CONTEXT_RESTORE_ARCH_CORE  xc:req pc:req
  movq OFFSET_R15(\xc), %r15
  movq OFFSET_R14(\xc), %r14
  movq OFFSET_R13(\xc), %r13
  movq OFFSET_R12(\xc), %r12
  movq OFFSET_RBP(\xc), %rbp
  movq OFFSET_RBX(\xc), %rbx
  movq OFFSET_SP(\xc), %rsp
  movq OFFSET_PC(\xc), \pc
.endm

#endif /* __ASM__ */

/* amd64 specific note: ABI describes that stack
 * must be aligned to 16 byte , this can affect va_arg and so on ...
 */
#define SP_DELTA  16

#ifndef __ASM__

#include <eza/arch/types.h>

typedef struct __context_t { /* others don't interesting... */
  uintptr_t sp;
  uintptr_t pc;

  uint64_t rbx;
  uint64_t rbp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;

  ipl_t ipl;
} __attribute__ ((packed)) context_t;

#endif /* __ASM__ */

#endif /* __ARCH_CONTEXT_H__ */

