#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Delete */
void test_delete(void) {

	int data1 = 3, data2 = 4;
	char data3 = 'c';
    double data4 = 5.0, *ptr;

	queue_t q;

	fprintf(stderr, "*** TEST delete ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
    queue_enqueue(q, &data4);

    queue_delete(q, &data3);
    queue_delete(q, &data2);
    queue_delete(q, &data1);

    queue_dequeue(q, (void**)&ptr);

    TEST_ASSERT(ptr == &data4);
	
}

/* Iterate */

static void iterator_print(queue_t q, void *data)
{
	int *a = (int*)data;

	printf("%d\n", *a);
	queue_length(q);
	
}

void test_iterate() {

	int data1 = 3, data2 = 4, data3 = 5;
	queue_t q;
	

	fprintf(stderr, "*** TEST iterate ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_iterate(q, iterator_print);


}

int main(void)
{
	test_create();
	test_queue_simple();
	test_delete();
	test_iterate();

	return 0;
}
