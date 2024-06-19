#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <gst/gst.h>

#define PIPELINE_TYPE_RT_POOL (pipeline_rt_pool_get_type ())

typedef struct _PipelineRTPool PipelineRTPool;
typedef struct _PipelineRTPoolClass PipelineRTPoolClass;

struct _PipelineRTPool {
    GstTaskPool parent;
    GThreadPool *pool;
};

struct _PipelineRTPoolClass {
    GstTaskPoolClass parent_class;
};

typedef struct {
    gpointer data;
    int priority;
} DataPriority;

typedef struct {
  pthread_t thread;
} PipelineRTId;

static void thread_pool_prepare(GstTaskPool *pool, GError **error);
static void thread_pool_cleanup(GstTaskPool *pool);
static void thread_pool_push(GstTaskPool *pool, GstTaskPoolFunction func, gpointer data, GError **error);
static void thread_pool_join(GstTaskPool *pool, gpointer id);

static void pipeline_rt_pool_class_init(PipelineRTPoolClass* klass);
static void pipeline_rt_pool_init(PipelineRTPool* pool);
GstTaskPool *pipeline_rt_pool_new();

#endif // __THREADPOOL_H__