#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct item_t {
	void* data;
	struct item *next;
} item, *item_p;

struct queue {
	/* https://stackoverflow.com/questions/10768947/typedef-struct-pointer-definition */
	item_p head;
	item_p tail;
	int length;
};

queue_t queue_create(void)
{
	/* https://computer.howstuffworks.com/c31.htm */
	queue_t newQueuePtr = (queue_t) malloc(sizeof(queue));

	if (newQueuePtr == NULL){
		return NULL;
	}

	newQueue->head = NULL;
	newQueue->head = NULL;
	newQueue->length = 0;
	return newQueuePtr;
}

int queue_destroy(queue_t queue)
{
	if (!queue || queue->length > 0){
		return -1;
	}
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (!queue || !data){
		return -1; 
	}

	item_p newItemPtr = (itemp_p) malloc(sizeof(item));
	if (!newItemPtr){
		return -1;
	}

	newItemPtr->data = data;
	newItemPtr->next = NULL;
	if (queue->head == NULL){
		queue->head = newItemPtr;
		queue->tail = newItemPtr;
	} 
	else {
		queue->tail->next = newItemPtr;
		queue->tail = newItemPtr;
	}

	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* https://github.com/kostakis/Generic-Queue/blob/master/queue.c */
	if (!queue || !data || !queue->head){
		return -1;
	}

	item_p oldestItem = queue->head;
	*data = oldestItem->data;
	queue->head = oldestItem->next;
	free(oldestItem);

	queue->length--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	// OH - can I ask: do we check if pointers point to same memory? 
	if (!queue || !data ){
		return -1;
	}

	/*item_p iterItem;
	for (iterItem = queue->head; iterItem != NULL; iterItem = iterItem->next){
		if ()
	}*/

	queue->length--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* https://riptutorial.com/c/example/31818/typedef-for-function-pointers */
	if (!queue || !func ){
		return -1;
	}
	
	void (*func)()
	item_p iterItem;
	for (iterItem = queue->head; iterItem != NULL; iterItem = iterItem->next){
		func(queue, iterItem->data);
	}

	iterItem = NULL;
	return 0;
}

int queue_length(queue_t queue)
{
	if (!queue){
		return -1;
	}

	return queue->length;
}