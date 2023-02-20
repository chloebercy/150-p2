/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <uthread.h>
#include <private.h>

void hello(void *arg)
{
	(void)arg;

	printf("Hello world!\n");

	int i = 0;
	while (i<1000000) {
		if (i % 100 == 0)
			printf(".");
		i++;
	}

	i = 0;
	printf("\n\n\nHello world! 2\n");
	preempt_disable();
	while (i<1000000) {
		if (i % 100 == 0)
			printf(".");
		i++;
	}
	preempt_enable();
}

int main(void)
{
	uthread_run(true, hello, NULL);

	printf("\n");
	return 0;
}
