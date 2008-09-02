
#ifndef __PAGEACCS_H__
#define __PAGEACCS_H__ 

#include <eza/arch/types.h>

/* Context for linear area page accessor. */
typedef struct __pageaccs_linear_pa_ctx {
  page_idx_t start_page, end_page, curr_page;
} pageaccs_linear_pa_ctx_t;

/* Linear area page accessor that uses 'pageaccs_linear_pa_ctx_t'
 * as its context. */
extern page_frame_accessor_t pageaccs_linear_pa;

/* Stubs for simple page accesses. */
void pageaccs_reset_stub(void *ctx);
page_frame_t *pageaccs_alloc_page_stub(void *ctx,page_flags_t flags,int clean_page);
page_idx_t pageaccs_frames_left_stub(void *ctx);


#endif

