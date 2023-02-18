#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t blocked_q;
	size_t count; // should this be a pointer?
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

	newSem->count = count; //newSem->count = (size_t*)malloc(sizeof(size_t));
	
	return newSem;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->blocked_q) != 0){
		return -1;
	}

	queue_destroy(sem->blocked_q);
	sem = NULL;
	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL){
		return -1;
	}

	while (sem->count == 0) {
		queue_enqueue(sem->blocked_q, uthread_current());
		uthread_block();
	} // how to avoid starvation here?

	sem->count -= 1;
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL){
		return -1;
	}

	sem->count += 1;

	if (queue_length(sem->blocked_q) != 0){
		struct uthread_tcb *uthread_next;
		queue_dequeue(sem->blocked_q, (void**)&uthread_next);
		uthread_unblock(uthread_next);
	}

	return 0;
}

