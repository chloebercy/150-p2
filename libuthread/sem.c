#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // delete later

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t blocked_q;
	size_t count; // should this be a pointer?
	bool starvation_prevention;
};

sem_t sem_create(size_t count)
{
	//initializing a new semaphore
	sem_t newSem = (sem_t)malloc(sizeof(struct semaphore));

	if (newSem == NULL){
		return NULL;
	}
	
	queue_t newQueue = queue_create();
	newSem->blocked_q = newQueue; 
	newSem->starvation_prevention = false;

	newSem->count = count; //newSem->count = (size_t*)malloc(sizeof(size_t));
	
	return newSem;
}

int sem_destroy(sem_t sem)
{
	// error handling
	if (sem == NULL || queue_length(sem->blocked_q) != 0){
		return -1;
	}

	// free blocked queue memory && set semaphore pointer to NULL
	queue_destroy(sem->blocked_q);
	sem = NULL;
	return 0;
}

int sem_down(sem_t sem)
{
	// error handling
	if (sem == NULL){
		return -1;
	}

	// check if another thread is starved of this resource
	if (sem->starvation_prevention == true){
		queue_enqueue(sem->blocked_q, uthread_current());
		uthread_block();
	}

	// if semaphore unavailable, block thread until becomes available
	int blocked_count = 0;
	while (sem->count == 0) {
		if (blocked_count >= 0){ blocked_count++; }

		if (blocked_count > 2){
			sem->starvation_prevention = true;
			blocked_count = -1;
		}

		queue_enqueue(sem->blocked_q, uthread_current());
		uthread_block();
	}

	if (blocked_count < 0){ sem->starvation_prevention = false; }

	sem->count -= 1;
	return 0;
}

int sem_up(sem_t sem)
{
	// error handling
	if (sem == NULL){
		return -1;
	}

	// increase semaphore count
	sem->count += 1;

	// unnblock first thread in blocked queue
	if (queue_length(sem->blocked_q) != 0){
		struct uthread_tcb *uthread_next;
		queue_dequeue(sem->blocked_q, (void**)&uthread_next);
		uthread_unblock(uthread_next);
	}

	return 0;
}

