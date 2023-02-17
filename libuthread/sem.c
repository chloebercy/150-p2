#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	queue_t *q;
	size_t *count;
	int *lock;
};

sem_t sem_create(size_t count)
{
	//initializing a new semaphore
	sem_t *newSem = (sem_t*)malloc(sizeof(sem_t));
	newSem->count = count;

	return newSem;
}

int sem_destroy(sem_t sem)
{
	free(sem);
}

int sem_down(sem_t sem)
{
	// waiting for the count to become positive again and then decerements

	sem->lock = 1; // equivalent to spinlock_lock(sem->lock) ?? i think
	while (sem->count == 0) {
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

