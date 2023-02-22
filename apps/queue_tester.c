#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

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

/* Destroy */
void test_destroy_simple(void)
{
	fprintf(stderr, "*** TEST destroy_simple ***\n");
	
	queue_t q; 
	q = queue_create();

	TEST_ASSERT(queue_destroy(q) == 0);
}

void test_destroy_length_error(void)
{
	fprintf(stderr, "*** TEST destroy_length_error ***\n");
	
	int data = 3;
	queue_t q; 
	q = queue_create();
	queue_enqueue(q, &data);

	TEST_ASSERT(queue_destroy(q) == -1);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	fprintf(stderr, "*** TEST queue_simple ***\n");

	int data = 3, *ptr;
	queue_t q;
	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(ptr == &data);
}

/* Delete */
void test_delete(void) 
{
	fprintf(stderr, "*** TEST delete ***\n");

	int data1 = 3, data2 = 4;
	char data3 = 'c';
    double data4 = 5.0, *ptr;
	queue_t q;
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

/* Delete non-existing item */
void test_delete_error(void){
	fprintf(stderr, "*** TEST delete_error ***\n");

	int data1 = 3, data2 = 4;
	char data3 = 'c';
	queue_t q;
	q = queue_create();

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);

	TEST_ASSERT(queue_delete(q, &data3) == -1);

}


/*static void delete(queue_t q, void *data)
{
	queue_delete(q, data);
	fprintf(stderr, "?");
}*/

static void iterator_print(queue_t q, void *data)
{
	int total = queue_length(q);
	int *a = (int*)data;
	fprintf(stderr, "%d/%d\n", *a, total);
}

/* Iterate + Delete */
void test_iterate() 
{
	fprintf(stderr, "*** TEST iterate ***\n");

	int data1 = 1, data2 = 2, data3 = 3;
	queue_t q;
	q = queue_create();

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_iterate(q, iterator_print);
;}

int main(void)
{
	test_create();
	test_destroy_simple();
	test_destroy_length_error();
	test_queue_simple();
	test_delete();
	test_delete_error();
	test_iterate();

	return 0;
}
