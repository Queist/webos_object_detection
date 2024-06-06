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
    /*
    GstBuffer *new_buffer;
    GstMemory *memory;
    gint size = 1024 * 1024; // 1MB buffer

    // Allocate a new buffer and memory block
    new_buffer = gst_buffer_new();
    memory = gst_allocator_alloc(NULL, size, NULL);
    gst_buffer_append_memory(new_buffer, memory);

    *buffer = new_buffer;*/
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
    pool = g_object_new(PIPELINE_TYPE_BUFFER_POOL, NULL);
    return pool;
}