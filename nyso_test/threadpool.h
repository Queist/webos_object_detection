#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <gst/gst.h>

#define PIPELINE_TYPE_RT_POOL (pipeline_rt_pool_get_type())

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

static void pipeline_rt_pool_class_init(PipelineRTPoolClass* klass);
static void pipeline_rt_pool_init(PipelineRTPool* pool);
GstTaskPool *pipeline_rt_pool_new();

#endif // THREADPOOL_H