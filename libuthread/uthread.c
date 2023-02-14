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
	/*ucontext_t uthread_next;
	stack_t uthread_stack;
	mcontext_t uthread_registers;*/
	uthread_ctx_t uthread_ctx; 
	int state; // needed?
};

uthread_tcb **current = NULL;
queue_t *q = NULL;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return *current;
}

void uthread_yield(void)
{
	uthread_ctx_t *uthread_ctx_next;
	queue_dequeue(*q, (void**)&uthread_ctx_next);
	uthread_ctx_switch(uthread_current()->uthread_ctx, 
		uthread_ctx_next); // will trigger uthread_ctx_next execution
}

void uthread_exit(void)
{
	uthread_ctx_destroy_stack(uthread_current()->uthread_ctx->uc_stack);
	queue_delete(*q, uthread_current->uthread_ctx);
	free(**current); // free uthread_tcb struct pointed to by current
	current = NULL;  
}

int uthread_create(uthread_func_t func, void *arg)
{
	uthread_tcb *newThread = (uthread_tcb*)malloc(sizeof(uthread_tcb));

	void *top_of_stack = uthread_ctx_alloc_stack();
	uthread_ctx_init(newThread->uthread_ctx, top_of_stack, func, arg);

	queue_enqueue(*q, newThread->uthread_ctx);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// create uthread for main/idle and then create new queue
	uthread_ctx_t *idle = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	*q = queue_create();

	// create first thread
	uthread_create(func, arg);
	queue_enqueue(*q, idle); 
	// should we be just queining uthread_ctx_t types isntead of struct tcb? i think..

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

