/*
 * Semaphore starvation test
 *
 * Test whether semaphore will correctly handle starved threads.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <sem.h>
#include <uthread.h>

sem_t sem1;

static void thread7(void *arg)
{
	(void)arg;

	printf("thread7 : sem_down\n");
	sem_down(sem1);
	printf("thread7 : exit\n");
}

static void thread6(void *arg)
{
	(void)arg;

	printf("thread6 : sem_up\n");
	sem_up(sem1);
	printf("thread6 : exit\n");
}

static void thread5(void *arg)
{
	(void)arg;

	uthread_create(thread6, NULL);
	uthread_create(thread7, NULL);

	printf("thread5 : sem_down\n");
	sem_down(sem1);
	printf("thread5 : exit\n");
}

static void thread4(void *arg)
{
	(void)arg;

	printf("thread4 : sem_up\n");
	sem_up(sem1);
	printf("thread4 : exit\n");
}

static void thread3(void *arg)
{
	(void)arg;

	uthread_create(thread4, NULL);
	uthread_create(thread5, NULL);

	printf("thread3 : sem_down\n");
	sem_down(sem1);
	printf("thread3 : exit\n");
}

static void thread2(void *arg)
{
	(void)arg;

	printf("thread2 : sem_up\n");
	sem_up(sem1);
	printf("thread2 : exit\n");
}

static void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_create(thread3, NULL);

	printf("thread1 : sem_down\n");
	sem_down(sem1);
	printf("thread1 : sem_up\n");
	sem_up(sem1);
	printf("thread1 : exit\n");
}

int main(void)
{
	sem1 = sem_create(0);

	uthread_run(false, thread1, NULL);

	sem_destroy(sem1);

	return 0;
}
