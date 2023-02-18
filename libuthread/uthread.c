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
	//int state; // 0 = running, 1 = ready, -1 = blocked
};

struct uthread_tcb **current = NULL; 
queue_t *ready_q = NULL;

struct uthread_tcb *uthread_current(void)
{
	return *current;
	/* ADD TODO Phase 3 */
}

void uthread_yield(void)
{
	// set ctx_prev pointer to current context
	struct uthread_tcb *uthread_prev = uthread_current();

	// set ctx_next pointer to next item in queue & update current
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current = &uthread_next;
	queue_enqueue(*ready_q, uthread_prev);

	// swapcontext()
	uthread_ctx_switch(uthread_prev->uthread_ctx, uthread_next->uthread_ctx);
}

void uthread_exit(void)
{
	// destroy current tcb stack
	uthread_ctx_destroy_stack(uthread_current()->uthread_ctx->uc_stack.ss_sp);

	// free context pointed to by current
	free(uthread_current()->uthread_ctx); 

	// check if queue is empty 
	if(queue_length(*ready_q) < 1){
		return;
	}
	
	// if more threads, switch to next thread in queue, update current
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current = &uthread_next;
	setcontext(uthread_next->uthread_ctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// create newThread, initalize stack, makecontext(), add to queue;
	struct uthread_tcb *newTCB = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	newTCB->uthread_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

	void *top_of_stack = uthread_ctx_alloc_stack();
	uthread_ctx_init(newTCB->uthread_ctx, top_of_stack, func, arg);
	queue_enqueue(*ready_q, newTCB);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// to shut up unused parameter error
	preempt = 1; 
	printf("test print preempt 1 = %d\n", preempt);

	// initialize global queue pointer ready_q
	queue_t newQueue = queue_create();
	ready_q = &newQueue;

	// create uthread for main/idle 
	struct uthread_tcb *idle = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	idle->uthread_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

	// set global var struct double pointer "current" to TCB of idle 
	current = &idle;

	// create first thread & queue
	uthread_create(func, arg);

	// wait until queue is empty, strings all exited
	while(queue_length(*ready_q) > 0){
		uthread_yield();
	}
	return 0;
}

void uthread_block(void)
{
	// set ctx_prev pointer to current context
	struct uthread_tcb *uthread_prev = uthread_current();

	// set ctx_next pointer to next item in queue & update current
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current = &uthread_next;

	// swapcontext()
	uthread_ctx_switch(uthread_prev->uthread_ctx, uthread_next->uthread_ctx);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	queue_enqueue(*ready_q, uthread);
}