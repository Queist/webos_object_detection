#include <pthread.h>
#include "threadpool.h"

G_DEFINE_TYPE(PipelineRTPool, pipeline_rt_pool, GST_TYPE_TASK_POOL);

// Function to handle task
static void thread_func(gpointer data, gpointer user_data) {
    g_print("Thread function starts\n");
    GstTask *task = (GstTask *) data;
    DataPriorityFunc *dpf;

    // Call the actual function
    if (task) {
        dpf = (DataPriorityFunc *)(task->user_data);
        GstTaskFunction func = (dpf->func);
        g_print("priority check: %d\n", dpf->priority);
        if (func) {
            task->user_data = dpf->data;
            func(task);
            g_print("Thread function completed\n");
        }
    }
}

// Compare function for sorting tasks by priority
static gint compare_priority(gconstpointer a, gconstpointer b, gpointer user_data) {
    GstTask *task1 = (GstTask *) a;
    GstTask *task2 = (GstTask *) b;

    DataPriorityFunc *dpf1 = (DataPriorityFunc *)(task1->user_data);
    DataPriorityFunc *dpf2 = (DataPriorityFunc *)(task2->user_data);
    return dpf2->priority - dpf1->priority;
}


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
    PipelineRTPool *rt_pool = (PipelineRTPool *) pool;

    // Set func in user_data
    GstTask *task = (GstTask *) data;
    DataPriorityFunc *dpf = (DataPriorityFunc *)(task->user_data);
    dpf->func = func;

    g_print("Priority: %d\n", dpf->priority);

    // Push task to thread pool
    if(!g_thread_pool_push(rt_pool->pool, task, error)) {
        g_printerr("Error pushing task to thread pool: %s\n", (*error)->message);
    }

    return NULL;
}

static void thread_pool_join(GstTaskPool *pool, gpointer id) {
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

GstTaskPool *pipeline_rt_pool_new(int num_threads) {
    PipelineRTPool *rt_pool;
    rt_pool = (PipelineRTPool *) g_object_new(PIPELINE_TYPE_RT_POOL, NULL);

    // Create thread pool
    rt_pool->pool = g_thread_pool_new((GFunc) thread_func, NULL, num_threads, TRUE, NULL);
    g_thread_pool_set_sort_function(rt_pool->pool, compare_priority, NULL);

    return (GstTaskPool *) rt_pool;
}