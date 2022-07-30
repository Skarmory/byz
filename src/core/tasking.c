#include "core/tasking.h"

#include "core/list.h"
#include "core/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#define MAX_THREADS 4

struct Tasker* g_tasker = NULL;

struct Thread;
static void _thread_init(struct Thread* thread, struct Tasker* tasker);
static void _thread_free(struct Thread* thread);
static void _thread_execute_task(struct Thread* thread);
static void _thread_end_task(struct Thread* thread);
static void _thread_stop(struct Thread* thread);
static int  _thread_update(struct Thread* thread);

enum ThreadState
{
    THREAD_STATE_EXECUTING,
    THREAD_STATE_IDLE,
    THREAD_STATE_STOPPING,
    THREAD_STATE_STOPPED
};

struct Thread
{
    struct Tasker* tasker;
    thrd_t         thread;
    atomic_int     state;
    struct Task*   task;
    int            id;
};

struct Tasker
{
    atomic_int    pending_task_count;
    atomic_int    executing_task_count;
    atomic_int    completed_task_count;

    struct Thread worker_threads[MAX_THREADS];
    mtx_t         pending_list_lock;
    mtx_t         complete_list_lock;
    cnd_t         work_signal;

    struct List   pending_list;
    struct List   complete_list;
};

struct Task
{
    task_func          func;
    task_callback_func cb_func;
    void*              args;
    atomic_int         status;
    char               name[256];
};

/**
 * Set initial state for a worker thread.
 */
static void _thread_init(struct Thread* thread, struct Tasker* tasker)
{
    thread->tasker = tasker;
    thread->state = THREAD_STATE_IDLE;

    thrd_create(&thread->thread, (void*)_thread_update, thread);
}

/**
 * Stop a worker thread and join.
 */
static void _thread_free(struct Thread* thread)
{
    _thread_stop(thread);

    thrd_join(thread->thread, NULL);
}

/**
 * SHOULD ONLY BE CALLED BY A WORKER THREAD
 * Loops executing the given task until task returns a non-executing state.
 */
static void _thread_execute_task(struct Thread* thread)
{
    //log_format_msg(LOG_DEBUG, "Worker thread %d executing task: %s", thread->id, thread->task->name);

    ++thread->tasker->executing_task_count;

    thread->task->status = TASK_STATUS_EXECUTING;
    while(thread->task->status == TASK_STATUS_EXECUTING)
    {
        thread->task->status = thread->task->func(thread->task->args);
    }
}

/**
 * SHOULD ONLY BE CALLED BY A WORKER THREAD
 * Add a completed task to the tasker's completed task list.
 */
static void _thread_end_task(struct Thread* thread)
{
    //log_format_msg(LOG_DEBUG, "Worker thread %d ending task: %s", thread->id, thread->task->name);

    if(thread->task->cb_func)
    {
        thread->task->cb_func(thread->task->args);
    }

    task_free(thread->task);
    thread->task = NULL;
}

/**
 * SHOULD ONLY BE CALLED BY THE TASKER
 * Signal a thread to stop.
 */
static void _thread_stop(struct Thread* thread)
{
    enum ThreadState expect = THREAD_STATE_STOPPED;
    while(!atomic_compare_exchange_weak(&thread->state, &expect, THREAD_STATE_STOPPED))
    {
        expect = THREAD_STATE_STOPPED;
        cnd_signal(&thread->tasker->work_signal);
    }
}

/**
 * Main loop for a worker thread.
 * Lock the pending list, if there are tasks pop the top and begin.
 * If no tasks, wait on signal from the tasker that new tasks are added.
 * Atomic compare on state swaps in case the tasker stops the thread.
 */
static int _thread_update(struct Thread* thread)
{
    enum ThreadState expect = thread->state;
    while(atomic_compare_exchange_weak(&thread->state, &expect, THREAD_STATE_IDLE))
    {
        mtx_lock(&thread->tasker->pending_list_lock);

        while(thread->tasker->pending_task_count == 0)
        {
            // No pending tasks, wait for work signal
            cnd_wait(&thread->tasker->work_signal, &thread->tasker->pending_list_lock);

            // Check that we're still in the idle state
            // If not, we must be in the stopped state, so break out
            expect = THREAD_STATE_IDLE;
            if(!atomic_compare_exchange_strong(&thread->state, &expect, THREAD_STATE_IDLE))
            {
                mtx_unlock(&thread->tasker->pending_list_lock);
                goto thread_update_exit_label;
            }
        }

        thread->task = list_pop_head(&thread->tasker->pending_list);
        --thread->tasker->pending_task_count;

        mtx_unlock(&thread->tasker->pending_list_lock);

        // Check still idle, if so set executing, else stopped so break
        expect = THREAD_STATE_IDLE;
        if(!atomic_compare_exchange_strong(&thread->state, &expect, THREAD_STATE_EXECUTING))
        {
            goto thread_update_exit_label;
        }

        _thread_execute_task(thread);
        _thread_end_task(thread);

        // Check still executing, if so set idle, else stopped to break
        expect = THREAD_STATE_EXECUTING;
        if(!atomic_compare_exchange_strong(&thread->state, &expect, THREAD_STATE_IDLE))
        {
            goto thread_update_exit_label;
        }

        --thread->tasker->executing_task_count;

        expect = THREAD_STATE_IDLE;
    }

thread_update_exit_label:
    thread->state = THREAD_STATE_STOPPED;
    //log_format_msg(LOG_DEBUG, "Thread %d exited", thread->id);
    thrd_exit(0);
}

/**
 * Create a new tasker and set the initial state.
 * This creates the worker threads for the tasker.
 */
struct Tasker* tasker_new(void)
{
    struct Tasker* tasker = malloc(sizeof(struct Tasker));
    tasker->pending_task_count = 0;
    tasker->executing_task_count = 0;
    tasker->completed_task_count = 0;
    list_init(&tasker->pending_list);
    list_init(&tasker->complete_list);

    mtx_init(&tasker->pending_list_lock, mtx_plain);
    mtx_init(&tasker->complete_list_lock, mtx_plain);
    cnd_init(&tasker->work_signal);

    for(int i = 0; i < MAX_THREADS; ++i)
    {
        _thread_init(&tasker->worker_threads[i], tasker);
        tasker->worker_threads[i].id = i;
    }

    return tasker;
}

/**
 * Destroy the tasker and its internal state.
 * This also stops and joins the tasker's worker threads.
 */
void tasker_free(struct Tasker* tasker)
{
    for(int tidx = 0; tidx < MAX_THREADS; ++tidx)
    {
        tasker->worker_threads[tidx].state = THREAD_STATE_STOPPING;
    }

    for(int tidx = 0; tidx < MAX_THREADS; ++tidx)
    {
        _thread_free(&tasker->worker_threads[tidx]);
    }

    mtx_destroy(&tasker->pending_list_lock);
    mtx_destroy(&tasker->complete_list_lock);
    cnd_destroy(&tasker->work_signal);

    struct ListNode *node, *next;
    list_for_each_safe(&tasker->pending_list, node, next)
    {
        task_free(node->data);
        free(node);
    }

    list_for_each_safe(&tasker->complete_list, node, next)
    {
        task_free(node->data);
        free(node);
    }

    free(tasker);
}

/**
 * Add a task to the tasker's list, and signal a thread to awaken.
 */
bool tasker_add_task(struct Tasker* tasker, struct Task* task)
{
    if(task->status != TASK_STATUS_NOT_STARTED)
    {
        return false;
    }

    // Add task to pending list
    mtx_lock(&tasker->pending_list_lock);
    {
        list_add(&tasker->pending_list, task);
        ++tasker->pending_task_count;
    }
    mtx_unlock(&tasker->pending_list_lock);

    cnd_signal(&tasker->work_signal);

    return true;
}

/**
 * Block until tasker has finished executing all its pending tasks.
 */
void tasker_sync(struct Tasker* tasker)
{
    while(tasker->pending_task_count > 0 || tasker->executing_task_count > 0)
    {
        // Wait for tasker idle
        thrd_yield();
    }
}

/**
 * Return true if tasker has any tasks that it has not started yet.
 */
bool tasker_has_pending_tasks(struct Tasker* tasker)
{
    return tasker->pending_task_count != 0;
}

/**
 * Return true if tasker has any tasks that it is currently executing.
 */
bool tasker_has_executing_tasks(struct Tasker* tasker)
{
    return tasker->executing_task_count != 0;
}

/**
 * Return true if tasker has any tasks that it has completed, awaiting integration.
 */
bool tasker_has_completed_tasks(struct Tasker* tasker)
{
    return tasker->completed_task_count != 0;
}

/**
 * Log tasker state for debugging purposes.
 */
void tasker_log_state(struct Tasker* tasker)
{
    log_msg(LOG_DEBUG, "Tasker state:");
    log_push_indent(LOG_ID_DEBUG);
    log_format_msg(LOG_DEBUG, "Pending tasks: %d", tasker->pending_task_count);
    log_format_msg(LOG_DEBUG, "Executing tasks: %d", tasker->executing_task_count);
    log_format_msg(LOG_DEBUG, "Completed tasks: %d", tasker->completed_task_count);

    log_msg(LOG_DEBUG, "Threads:");
    log_push_indent(LOG_ID_DEBUG);
    for(int i = 0; i < MAX_THREADS; ++i)
    {
        log_format_msg(LOG_DEBUG, "Thread %d", tasker->worker_threads[i].id);
        log_push_indent(LOG_ID_DEBUG);
        log_format_msg(LOG_DEBUG, "State: %d", tasker->worker_threads[i].state);
        log_pop_indent(LOG_ID_DEBUG);
    }
    log_pop_indent(LOG_ID_DEBUG);
    log_pop_indent(LOG_ID_DEBUG);
}

/**
 * Create a new task and set its initial state
 */
struct Task* task_new(char* task_name, task_func func, task_func cb_func, void* args, int size_bytes)
{
    struct Task* task = malloc(sizeof(struct Task));
    task->status = TASK_STATUS_NOT_STARTED;
    task->func = func;
    task->cb_func = cb_func;
    task->args = malloc(size_bytes);
    memcpy(task->args, args, size_bytes);
    snprintf(task->name, sizeof(task->name), "%s", task_name);

    return task;
}

/**
 * Destroy a task.
 */
bool task_free(struct Task* task)
{
    if(task->status == TASK_STATUS_EXECUTING)
    {
        return false;
    }

    free(task->args);
    free(task);

    return true;
}

/**
 * Return the task's execution function.
 */
task_func task_get_func(struct Task* task)
{
    return task->func;
}

/**
 * Return true if the task has finished.
 */
bool task_is_finished(struct Task* task)
{
    return task->status == TASK_STATUS_SUCCESS || task->status == TASK_STATUS_FAILED;
}
