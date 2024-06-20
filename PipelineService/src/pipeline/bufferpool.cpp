#include "bufferpool.h"
#include <gst/gst.h>

G_DEFINE_TYPE(PipelineBufferPool, pipeline_buffer_pool, GST_TYPE_BUFFER_POOL);

static gboolean buffer_pool_start(GstBufferPool *pool) {
    PipelineBufferPool *buffer_pool = (PipelineBufferPool *) pool;
    g_print("buffer pool %s initialize.\n", buffer_pool->config->owner);
    return gst_buffer_pool_set_active(&(buffer_pool->parent), TRUE);
}

static gboolean buffer_pool_stop(GstBufferPool *pool) {
    PipelineBufferPool *buffer_pool = (PipelineBufferPool *) pool;
    g_print("buffer pool %s stops.\n", buffer_pool->config->owner);
    return gst_buffer_pool_set_active(&(buffer_pool->parent), FALSE);
}

static GstFlowReturn buffer_pool_acquire_buffer(GstBufferPool *pool, GstBuffer **buffer, GstBufferPoolAcquireParams *params) {
    PipelineBufferPool *buffer_pool = (PipelineBufferPool *) pool;

    return gst_buffer_pool_acquire_buffer(&(buffer_pool->parent), buffer, params);
}


static void pipeline_buffer_pool_class_init(PipelineBufferPoolClass *klass) {
    GstBufferPoolClass *buffer_pool_class = GST_BUFFER_POOL_CLASS(klass);

    buffer_pool_class->start = buffer_pool_start;
    buffer_pool_class->stop = buffer_pool_stop;
    buffer_pool_class->acquire_buffer = buffer_pool_acquire_buffer;
}

static void pipeline_buffer_pool_init(PipelineBufferPool *pool) {
}

GstBufferPool *pipeline_buffer_pool_new(gchar *owner, guint size, guint min_buf, guint max_buf) {
    PipelineBufferPool *buffer_pool;
    buffer_pool = (PipelineBufferPool *) g_object_new(PIPELINE_TYPE_BUFFER_POOL, NULL);
    GstStructure *config = gst_structure_new_empty("config");
    gst_buffer_pool_config_set_params(config, NULL, size, min_buf, max_buf);

    buffer_pool->config->owner = owner;

    buffer_pool->parent = *(gst_buffer_pool_new());
    gst_buffer_pool_set_config(&(buffer_pool->parent), config);
    return (GstBufferPool *) buffer_pool;
}

