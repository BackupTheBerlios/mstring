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
 * (c) Copyright 2008 Dan Kruchinin <dan.kruchinin@gmail.com>
 *
 * include/eza/amd64/types.h: types definions
 *
 */

#ifndef __AMD64__TYPES_H__
#define __AMD64__TYPES_H__

#define NULL ((void *)0)
#define TRUE   1
#define FALSE  0
/* small letters defines */
#define false  0
#define true   1
#define nil    0x0
#define fil    0xffffffff

/* Some macro to make life a bit easier. */
#define KB(x) ((x)*1024)
#define MB(x) ((x)*1024*1024)

/* simple typedefs */
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef unsigned char uint8_t; /* unsigned */
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long ulong_t;
typedef signed long long_t;
typedef uint8_t bool;
/* system used types */
typedef uint64_t size_t;
typedef uint64_t count_t;
typedef uint64_t index_t;
typedef uint64_t uintptr_t;
typedef uint32_t pid_t;
typedef uint32_t usec_t;
typedef int32_t status_t;

/* abstraction typedef */
typedef uint64_t unative_t;
typedef uint64_t native_t;
typedef uint64_t ipl_t;

/* memory-related types. */
typedef uint32_t page_idx_t;
typedef uint32_t page_flags_t;

/* bit-related types. */
typedef uint32_t bit_idx_t;

/* SMP-related stuff. */
typedef uint32_t cpu_id_t;

/* PTE entry structure specific for amd64 */
typedef struct {
  unsigned present : 1;
  unsigned rw : 1;
  unsigned us : 1;
  unsigned pwt : 1;
  unsigned pcd : 1;
  unsigned a : 1;
  unsigned d : 1;
  unsigned pat: 1;
  unsigned g : 1;
  unsigned available_9_11: 3;
  unsigned base_0_19: 20;
  unsigned base_20_39: 20;
  unsigned available_52_62: 11;
  unsigned nx: 1;
} __attribute__ ((packed)) pte_t;


#endif

