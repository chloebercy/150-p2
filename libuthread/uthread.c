#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

struct uthread_tcb {
	uthread_ctx_t *uthread_ctx; 
	//int state;  
};

struct uthread_tcb **current = NULL; 
queue_t *q = NULL;

struct uthread_tcb *uthread_current(void)
{
	return *current;
	/* ADD TODO Phase 3 */
}

void uthread_yield(void)
{
	// set ctx_prev pointer to current context
	uthread_ctx_t *uthread_ctx_prev = uthread_current()->uthread_ctx;

	// set ctx_next pointer to next item in queue & update current
	uthread_ctx_t *uthread_ctx_next;
	queue_dequeue(*q, (void**)&uthread_ctx_next);
	(*current)->uthread_ctx = uthread_ctx_next;

	// swapcontext()
	uthread_ctx_switch(uthread_ctx_prev, uthread_ctx_next); // will trigger uthread_ctx_next execution
}

void uthread_exit(void)
{
	// destroy current tcb stack
	uthread_ctx_destroy_stack(&(uthread_current()->uthread_ctx->uc_stack));

	//queue_delete(*q, uthread_current()->uthread_ctx); should not be in queue

	// free context pointed to by current
	free(uthread_current()->uthread_ctx); 
}

int uthread_create(uthread_func_t func, void *arg)
{
	// create newThread, initalize stack, makecontext(), add to queue;
	uthread_ctx_t *newThread = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	
	void *top_of_stack = uthread_ctx_alloc_stack();
	uthread_ctx_init(newThread, top_of_stack, func, arg);
	queue_enqueue(*q, newThread);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// initialize global queue pointer q
	*q = queue_create();
	
	// create uthread for main/idle 
	uthread_ctx_t *idle = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

	// set global var struct double pointer "current" to TCB of idle 
	struct uthread_tcb *currentTCB = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	currentTCB->uthread_ctx = idle;
	current = &currentTCB;

	// create first thread & queue
	uthread_create(func, arg);

	// add idle to end of queue
	queue_enqueue(*q, idle); 

	uthread_yield();
	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

