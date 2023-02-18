#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t *blockedQ;
	size_t *count;
};

sem_t sem_create(size_t count)
{
	//initializing a new semaphore
	sem_t *newSem = (sem_t*)malloc(sizeof(sem_t));

	if (newSem == NULL){
		return NULL;
	}
	
	queue_t newQueue = queue_create();
	newSem->blockedQ = &newQueue;

	newSem->count = count;
	
	return newSem;
}

int sem_destroy(sem_t sem)
{
	if (semm == NULL || queue_length(sem->blockedQ)) != 0){
		return -1;
	}

	queue_destroy(*(sem->blockedQ));
	free(sem->count);
	free(&sem);
	return 0;
}

int sem_down(sem_t sem)
{
	// waiting for the count to become positive again and then decerements

	sem->lock = 1; // equivalent to spinlock_lock(sem->lock) ?? i think
	while (sem->count == 0) {
		queue_enqueue(sem->blockedQ, uthread_current()->uthread_ctx);
		uthread_block(); // what is the difference between yield and block?
	}
	sem->count -= 1;
	sem->lock = 0; // equivalent to spinlock_unlock(sem->lock)
}

int sem_up(sem_t sem)
{
	// only incremements the count

	sem->lock = 1; // lock
	sem->count += 1;
	/* begin the first thread in queue; last thread to call down() */


	sem->lock = 0; // unlock
}

