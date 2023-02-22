#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t blocked_q;
	size_t count;
	bool starvation_prevention;
};

sem_t sem_create(size_t count)
{
	// Initialize a new semaphore
	preempt_disable();
	sem_t newSem = (sem_t)malloc(sizeof(struct semaphore));
	preempt_enable();

	// Error Handling
	if (newSem == NULL){
		return NULL;
	}
	
	// Initialize or define member variables of struct semaphore accordingly
	preempt_disable();
	queue_t newQueue = queue_create();
	preempt_enable();
	newSem->blocked_q = newQueue; 
	newSem->starvation_prevention = false;
	newSem->count = count;
	
	return newSem;
}

int sem_destroy(sem_t sem)
{
	// Error handling
	if (sem == NULL || queue_length(sem->blocked_q) != 0){
		return -1;
	}

	// Free blocked queue memory & set semaphore pointer to NULL
	queue_destroy(sem->blocked_q);
	sem = NULL;

	return 0;
}

int sem_down(sem_t sem)
{
	// Error handling
	if (sem == NULL){
		return -1;
	}

	// Check if another thread is starved of this resource
	if (sem->starvation_prevention == true){
		preempt_disable();
		queue_enqueue(sem->blocked_q, uthread_current());
		preempt_enable();
		uthread_block();
	}

	// If semaphore unavailable, block thread until becomes available
	int blocked_count = 0;
	while (sem->count == 0) {
		if (blocked_count >= 0){ blocked_count++; }

		// Handling if thread has been blocked more than two times
		if (blocked_count > 2){
			sem->starvation_prevention = true;
			blocked_count = -1;
		}

		preempt_disable();
		queue_enqueue(sem->blocked_q, uthread_current());
		preempt_enable();
		uthread_block();
	}

	// Previously starved thread alerts semaphore starvation handling is no longer needed
	if (blocked_count < 0){ 
		preempt_disable();
		sem->starvation_prevention = false; 
		preempt_enable();
	}

	// Decrease semaphore count
	preempt_disable();
	sem->count -= 1;
	preempt_enable();

	return 0;
}

int sem_up(sem_t sem)
{
	// Error handling
	if (sem == NULL){
		return -1;
	}

	// Increase semaphore count
	preempt_disable();
	sem->count += 1;
	preempt_enable();

	// Unnblock first thread in blocked queue
	if (queue_length(sem->blocked_q) != 0){
		struct uthread_tcb *uthread_next;
		preempt_disable();
		queue_dequeue(sem->blocked_q, (void**)&uthread_next);
		preempt_enable();
		uthread_unblock(uthread_next);
	}

	return 0;
}

