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
queue_t *readyQ = NULL;

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
	queue_dequeue(*readyQ, (void**)&uthread_ctx_next);
	(*current)->uthread_ctx = uthread_ctx_next;
	queue_enqueue(*readyQ, uthread_ctx_prev);

	// swapcontext()
	uthread_ctx_switch(uthread_ctx_prev, uthread_ctx_next);
}

void uthread_exit(void)
{
	// destroy current tcb stack
	uthread_ctx_destroy_stack(uthread_current()->uthread_ctx->uc_stack.ss_sp);

	// free context pointed to by current
	free(uthread_current()->uthread_ctx); 

	// check if queue is empty 
	if(queue_length(*readyQ) < 1){
		return;
	}
	
	// if more threads, switch to next thread in queue, update current
	uthread_ctx_t *uthread_ctx_next;
	queue_dequeue(*readyQ, (void**)&uthread_ctx_next);
	(*current)->uthread_ctx = uthread_ctx_next;
	setcontext(uthread_ctx_next);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// create newThread, initalize stack, makecontext(), add to queue;
	uthread_ctx_t *newThread = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	
	void *top_of_stack = uthread_ctx_alloc_stack();
	uthread_ctx_init(newThread, top_of_stack, func, arg);
	queue_enqueue(*readyQ, newThread);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// to shut up unused parameter error
	preempt = 1; 
	printf("test print preempt 1 = %d\n", preempt);

	// initialize global queue pointer readyQ
	queue_t newQueue = queue_create();
	readyQ = &newQueue;

	// create uthread for main/idle 
	uthread_ctx_t *idle = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

	// set global var struct double pointer "current" to TCB of idle 
	struct uthread_tcb *currentTCB = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	currentTCB->uthread_ctx = idle;
	current = &currentTCB;

	// create first thread & queue
	uthread_create(func, arg);

	// wait until queue is empty, strings all exited
	while(queue_length(*readyQ) > 0){
		uthread_yield();
	}
	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	uthread->uthread_ctx = NULL; // to shut up error
}