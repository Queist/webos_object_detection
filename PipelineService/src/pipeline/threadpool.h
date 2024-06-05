#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

//#include <gst/gst.h>
#include <pthread.h>
#include "pipeline.h"

typedef struct
{
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