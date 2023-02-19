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
};

// Global Variables
struct uthread_tcb **current_uthread_tcb = NULL; 
queue_t *ready_q = NULL;


struct uthread_tcb *uthread_current(void)
{
	return *current_uthread_tcb;
}

void uthread_yield(void)
{
	// Set temporary uthread_tcb pointer to global variable @current_uthread_tcb
	struct uthread_tcb *uthread_prev = uthread_current();

	// Set temporary uthread_tcb pointer to next item in queue @ready_q & update @current_uthread_tcb
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current_uthread_tcb = &uthread_next;

	// Enqueue previous uthread @uthread_prev to queue @ready_q
	queue_enqueue(*ready_q, uthread_prev);

	// Context Switch
	uthread_ctx_switch(uthread_prev->uthread_ctx, uthread_next->uthread_ctx);
}

void uthread_exit(void)
{
	// Destroy @current_uthread_tcb stack
	uthread_ctx_destroy_stack(uthread_current()->uthread_ctx->uc_stack.ss_sp);

	// Free uthread_ctx pointer member variable of @current_uthread_tcb
	free(uthread_current()->uthread_ctx); 

	// Check if queue is empty 
	if(queue_length(*ready_q) < 1){
		return;
	}
	
	// Queue is not empty: context switch to next thread in queue, update @current_uthread_tcb
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current_uthread_tcb = &uthread_next;
	setcontext(uthread_next->uthread_ctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// Initalize newTCB pointer, uthread_ctx pointer, top of stack pointer
	struct uthread_tcb *newTCB = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	newTCB->uthread_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	void *top_of_stack = uthread_ctx_alloc_stack();

	// Error Handling
	if (newTCB == NULL || newTCB->uthread_ctx == NULL || top_of_stack == NULL){
		return -1;
	}

	// Initialize thread's execution context
	uthread_ctx_init(newTCB->uthread_ctx, top_of_stack, func, arg);

	// Enqueue newTCB in queue @ready_q
	queue_enqueue(*ready_q, newTCB);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// to shut up unused parameter error
	preempt = 1; 
	printf("test print preempt 1 = %d\n", preempt);

	// Initialize queue, Define global queue pointer @ready_q
	queue_t newQueue = queue_create();
	ready_q = &newQueue;

	// Create uthread_tcb for main/idle 
	struct uthread_tcb *idle = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	idle->uthread_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

	// Set @current_uthread_tcb to TCB of idle 
	current_uthread_tcb = &idle;

	// Create first thread
	uthread_create(func, arg);

	// Wait until queue @ready_q is empty, all threads have exited
	while(queue_length(*ready_q) > 0){
		uthread_yield();
	}

	return 0;
}

void uthread_block(void)
{
	// Set temporary uthread_tcb pointer to global variable @current_uthread_tcb
	struct uthread_tcb *uthread_prev = uthread_current();

	// Set temporary uthread_tcb pointer to next item in queue @ready_q & update @current_uthread_tcb
	struct uthread_tcb *uthread_next;
	queue_dequeue(*ready_q, (void**)&uthread_next);
	current_uthread_tcb = &uthread_next;

	// Context Switch
	uthread_ctx_switch(uthread_prev->uthread_ctx, uthread_next->uthread_ctx);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	// Enqueue thread TCB in the ready queue @ready_q
	queue_enqueue(*ready_q, uthread);
}