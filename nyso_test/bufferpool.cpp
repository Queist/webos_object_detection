#include "bufferpool.h"

G_DEFINE_TYPE(PipelineBufferPool, pipeline_buffer_pool, GST_TYPE_BUFFER_POOL);

static gboolean buffer_pool_start(GstBufferPool *pool) {
    /* TODO */
    return TRUE;
}

static gboolean buffer_pool_stop(GstBufferPool *pool) {
    /* TODO */
    return TRUE;
}


static GstFlowReturn buffer_pool_alloc_buffer(GstBufferPool *pool, GstBuffer **buffer, GstBufferPoolAcquireParams *params) {
    g_print("Allocating buffer\n");
    //TODO
    return GST_FLOW_OK;
}

static void pipeline_buffer_pool_class_init(PipelineBufferPoolClass *klass) {
    GstBufferPoolClass *gstbufferpool_class;

    gstbufferpool_class = (GstBufferPoolClass *) klass;

    gstbufferpool_class->start = buffer_pool_start;
    gstbufferpool_class->stop = buffer_pool_stop;
    gstbufferpool_class->alloc_buffer = buffer_pool_alloc_buffer;
}

static void pipeline_buffer_pool_init(PipelineBufferPool *pool) {
    /* TODO */
}

GstBufferPool *pipeline_buffer_pool_new() {
    GstBufferPool *pool;
    pool = (GstBufferPool *) g_object_new(PIPELINE_TYPE_BUFFER_POOL, NULL);
    return pool;
}