#ifndef __BUFFERPOOL_H__
#define __BUFFERPOOL_H__

static gboolean buffer_pool_start(GstBufferPool *pool);
static gboolean buffer_pool_stop(GstBufferPool *pool);
static GstFlowReturn buffer_pool_alloc_buffer(GstBufferPool *pool, GstBuffer **buffer, GstBufferPoolAcquireParams *params);

static void pipeline_buffer_pool_class_init(PipelineBufferPoolClass *klass);
static void pipeline_buffer_pool_init(PipelineBufferPool *pool);
GstBufferPool *pipeline_buffer_pool_new();

#endif // __BUFFERPOOL_H__