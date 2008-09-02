
#include <mm/pt.h>
#include <eza/pageaccs.h>
#include <mm/pagealloc.h>
#include <eza/swks.h>

void pageaccs_reset_stub(void)
{
}

page_frame_t *pageaccs_alloc_page_stub(page_flags_t flags,int clean_page)
{
  return alloc_page(flags,clean_page); 
}

page_idx_t pageaccs_frames_left_stub(void)
{
  return swks.mem_total_pages;
}

/*

typedef struct __page_frame_accessor {
  page_idx_t (*frames_left)(void);
  page_idx_t (*next_frame)(void);
  void (*reset)(void);
  page_frame_t *(*alloc_page)(page_flags_t flags,int clean_page);
} page_frame_accessor_t

*/



