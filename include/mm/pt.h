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
 * include/mm/pt.h: Contains types and prototypes for generic kernel page table
 *                  manipulations.
 */


#ifndef __PT_H__
#define __PT_H__

#include <eza/arch/types.h>
#include <eza/spinlock.h>
#include <eza/arch/page.h>
#include <eza/list.h>
#include <mm/mm.h>

/* Page that never exists */
#define INVALID_PAGE_IDX  0xffffffff

typedef struct __page_directory {
  spinlock_t lock;
  uint8_t *entries;  /* Must refer to a page that contains TLB entries
                      * for this page directory.
                      */
} page_directory_t;

/* Top-level page-table record. */
extern page_directory_t kernel_pt_directory;

typedef struct __page_frame_accessor {
  page_idx_t (*frames_left)(void *ctx);
  page_idx_t (*next_frame)(void *ctx);
  void (*reset)(void *ctx);
  page_frame_t *(*alloc_page)(void *ctx, page_flags_t flags,int clean_page);
} page_frame_accessor_t;

/**
 * @fninitialize_page_directory(page_directory_t *pd)
 *
 * Initializes target page-directory.
 *
 * @pd page-directory to be initialized.
 */
void initialize_page_directory(page_directory_t *pd);

/* Macro that uses initial top-level page directory as target directory. */
#define __mm_map_pages(pacc,virt_addr,num_pages,flags,ctx) \
               mm_map_pages(&kernel_pt_directory,pacc,virt_addr, \
                       num_pages, flags,ctx)
/**
 * @fn mm_map_pages( page_directory_t *top_level_pgd, uintptr_t phys_addr,
 *               uintptr_t virt_addr, size_t num_pages, page_flags_t flags ); 
 *
 * Maps number of physical pages starting at given physical address to
 * target virtual area, using target top-level page directory entry.
 *
 * @top_level_pgd Virtual address of target top-level page directory
 * @pacc Object that provides access to the sequence of physical pages
 *       to be mapped.
 * @virt_addr Target virtual address to map to (must be page-aligned)
 * @num_pages Number of pages to map
 * @flags Flags used for mapping pages (usually arch-specific)
 *
 * @return 0 on success,
 *         -ENOMEM if it was impossible to allocate memory for page tables,
 *         -EINVAL if insufficient number of pages or addresses were passed.
 */
int mm_map_pages( page_directory_t *top_level_pgd, page_frame_accessor_t *pacc,
                  uintptr_t virt_addr, size_t num_pages, page_flags_t flags,void *ctx );

/**
 * @fn mm_pin_virtual_address( page_directory_t *pd, uintptr_t virt_addr )
 *
 * Calculates a physical pages that is mapped to the given virtual address
 * for target page directory.
 *
 * @pd Page directory
 * @virt_addr Virtual address to be resolved.
 *
 * @return On success, a physical page number representing target virtual
 *         address is returned.
 *         Otherwise, INVALID_PAGE_IDX is returned.
 *
 */
page_idx_t mm_pin_virtual_address( page_directory_t *pd, uintptr_t virt_addr );

#endif
