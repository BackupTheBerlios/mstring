
#ifndef __PAGEACCS_H__
#define __PAGEACCS_H__ 

/* Stubs for simple page accesses. */
void pageaccs_reset_stub(void);
page_frame_t *pageaccs_alloc_page_stub(page_flags_t flags,int clean_page);
page_idx_t pageaccs_frames_left_stub(void);


#endif

