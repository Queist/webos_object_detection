#include <pthread.h>
#include "threadpool.h"

G_DEFINE_TYPE(PipelineRTPool, pipeline_rt_pool, GST_TYPE_TASK_POOL);

static void thread_pool_prepare(GstTaskPool *pool, GError **error) {
}

static void thread_pool_cleanup(GstTaskPool *pool) {
    PipelineRTPool *rt_pool = (PipelineRTPool *) pool;
    if (rt_pool->pool) {
        g_thread_pool_free(rt_pool->pool, TRUE, TRUE);
        rt_pool->pool = NULL;
    }
}

static gpointer thread_pool_push(GstTaskPool *pool, GstTaskPoolFunction func, gpointer data, GError **error) {
    PipelineRTId *pid = g_new0 (PipelineRTId, 1);
    gint res;
    pthread_attr_t attr;
    struct sched_param param;

    pid = g_new0 (PipelineRTId, 1);

    GstTask *task = (GstTask *) data;
    DataPriority *dp = (DataPriority *)(task->user_data);
    gpointer user_data = dp->data;
    int priority = dp->priority;

    task->user_data = user_data;

    g_print("Priority: %d\n", priority);

    if ((res = pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED)) != 0)
        g_warning ("setinheritsched: failure: %p", g_strerror (res));

    pthread_attr_init(&attr);
    if ((res = pthread_attr_setschedpolicy (&attr, SCHED_RR)) != 0)
        g_warning ("setschedpolicy: failure: %p", g_strerror (res));

    param.sched_priority = priority;
    if ((res = pthread_attr_getschedparam (&attr, &param)) != 0)
        g_warning ("getschedparam: failure: %p", g_strerror (res));

    res = pthread_create (&pid->thread, &attr, (void *(*)(void *)) func, task);

    if (res != 0) {
        g_set_error (error, G_THREAD_ERROR, G_THREAD_ERROR_AGAIN,
            "Error creating thread: %s", g_strerror (res));
        g_free (pid);
        pid = NULL;
    }

    return pid;
}

static void thread_pool_join(GstTaskPool *pool, gpointer id) {
    PipelineRTId *pid = (PipelineRTId *) id;
    pthread_join(pid->thread, NULL);
    g_free(pid);
}

static void pipeline_rt_pool_class_init(PipelineRTPoolClass* klass) {
    GstTaskPoolClass *gsttaskpool_class;

    gsttaskpool_class = (GstTaskPoolClass *) klass;

    gsttaskpool_class->prepare = thread_pool_prepare;
    gsttaskpool_class->cleanup = thread_pool_cleanup;
    gsttaskpool_class->push = thread_pool_push;
    gsttaskpool_class->join = thread_pool_join;
}

static void pipeline_rt_pool_init(PipelineRTPool* pool) {
}

GstTaskPool *pipeline_rt_pool_new() {
    GstTaskPool *pool;
    pool = (GstTaskPool *) g_object_new (PIPELINE_TYPE_RT_POOL, NULL);
    return pool;
}