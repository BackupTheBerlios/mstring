
#include <mm/pt.h>
#include <eza/pageaccs.h>
#include <mm/pagealloc.h>
#include <eza/swks.h>

void pageaccs_reset_stub(void *ctx)
{
}

page_frame_t *pageaccs_alloc_page_stub(void *ctx,page_flags_t flags,int clean_page)
{
  return alloc_page(flags,clean_page); 
}

page_idx_t pageaccs_frames_left_stub(void *ctx)
{
  return swks.mem_total_pages;
}

/* Linear area page accessor that uses 'pageaccs_linear_page_accessor_ctx_t'
 * as its context. */
static page_idx_t linear_pa_frames_left(void *ctx)
{
  pageaccs_linear_pa_ctx_t *lctx = (pageaccs_linear_pa_ctx_t*)ctx;
  if( lctx->curr_page < lctx->end_page ) {
    return lctx->curr_page - lctx->end_page;
  } else {
    return 0;
  }
}

static page_idx_t *linear_pa_next_frame(void *ctx)
{
  pageaccs_linear_pa_ctx_t *lctx = (pageaccs_linear_pa_ctx_t*)ctx;
  return lctx->curr_page++;
}

static page_idx_t *linear_pa_reset(void *ctx)
{
  pageaccs_linear_pa_ctx_t *lctx = (pageaccs_linear_pa_ctx_t*)ctx;
  lctx->curr_page = lctx->start_page;
}

page_frame_accessor_t pageaccs_linear_pa = {
  .frames_left = linear_pa_frames_left,
  .next_frame = linear_pa_next_frame,
  .reset = linear_pa_reset,
  .alloc_page = pageaccs_alloc_page_stub,
};

/*

typedef struct __page_frame_accessor {
  page_idx_t (*frames_left)(void);
  page_idx_t (*next_frame)(void);
  void (*reset)(void);
  page_frame_t *(*alloc_page)(page_flags_t flags,int clean_page);
} page_frame_accessor_t

*/



