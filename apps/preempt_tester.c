/*
 * Simple preemption test
 *
 * Tests the forceful "yielding" of threads when preemption is used.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <uthread.h>

#define MAXCOUNT 20

struct test {
	int maxcount;
};

void thread2(void *arg)
{
    struct test *t = (struct test*)arg;

    int i = 0;
	while (i < t->maxcount) {
		printf("2");
        sleep(1);
		i++;
	}
}

void thread1(void *arg)
{
	struct test *t = (struct test*)arg;

    uthread_create(thread2, arg);

	int i = 0;
	while (i < t->maxcount) {
		printf("1");
        sleep(1);
		i++;
	}
}

static unsigned int get_argv(char *argv)
{
	long int ret = strtol(argv, NULL, 0);
	if (ret == LONG_MIN || ret == LONG_MAX) {
		perror("strtol");
		exit(1);
	}
	return ret;
}

int main(int argc, char **argv)
{
    struct test t;
    int maxcount = MAXCOUNT;

    if (argc > 1)
		maxcount = get_argv(argv[1]);

	t.maxcount = maxcount;

    uthread_run(true, thread1, &t);

	printf("\n");
	return 0;
}


