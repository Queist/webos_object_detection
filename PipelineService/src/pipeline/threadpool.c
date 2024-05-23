#include <gst/gst.h>

static void thread_pool_prepare(GstTaskPool *pool, GError **error) {
    /* TODO */
}

static void thread_pool_cleanup(GstTaskPool *pool) {
    /* TODO */
}

static void thread_pool_push(GstTaskPool *pool, GstTaskPoolFunction func, gpointer data, GError **error) {
    /* TODO */
}

static void thread_pool_join(GstTaskPool *pool, gpointer id) {
    /* TODO */
}

static void pipeline_rt_pool_class_init(PipelineRTPoolClass* klass) {
    /* TODO */
}

static void pipeline_rt_pool_init(PipelineRTPool* pool) {
    /* TODO */
}

GstTaskPool *pipeline_rt_pool_new() {
    /* TODO */
    return NULL;
}