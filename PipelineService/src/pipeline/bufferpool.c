#include "bufferpool.h"

static gboolean buffer_pool_start(GstBufferPool *pool) {
    /* TODO */
    return TRUE;
}

static gboolean buffer_pool_stop(GstBufferPool *pool) {
    /* TODO */
    return TRUE;
}

static GstFlowReturn buffer_pool_alloc_buffer(GstBufferPool *pool, GstBuffer **buffer, GstBufferPoolAcquireParams *params) {
    /* TODO */
    return CUSTOM_SUCCESS_2;
}

static void pipeline_buffer_pool_class_init(PipelineBufferPoolClass *klass) {
    /* TODO */
}

static void pipeline_buffer_pool_init(PipelineBufferPool *pool) {
    /* TODO */
}

GstBufferPool *pipeline_buffer_pool_new() {
    /* TODO */
    return NULL;
}