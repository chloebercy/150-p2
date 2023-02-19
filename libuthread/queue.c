#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"


typedef struct item* item_p;

struct item {
	void* data;
	item_p next;
};

struct queue {
	/* https://stackoverflow.com/questions/10768947/typedef-struct-pointer-definition */
	item_p head;
	item_p tail;
	int length;
};

queue_t queue_create(void)
{
	/* https://computer.howstuffworks.com/c31.htm */
	queue_t newQueuePtr = (queue_t) malloc(sizeof(struct queue));
	if (newQueuePtr == NULL){
		return NULL;
	}
	newQueuePtr->head = NULL;
	newQueuePtr->head = NULL;
	newQueuePtr->length = 0;
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

	item_p newItemPtr = (item_p) malloc(sizeof(struct item));
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

int queue_enqueue_first(queue_t queue, void *data)
{
	if (!queue || !data){
		return -1; 
	}

	item_p newItemPtr = (item_p) malloc(sizeof(struct item));
	if (!newItemPtr){
		return -1;
	}

	newItemPtr->data = data;
	newItemPtr->next = queue->head;
	queue->head = newItemPtr;

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
	// OH / need to see - can we check if pointers point to same memory w/o undef behavior? 
	if (!queue || !data ){
		return -1;
	}

	item_p iterItem = queue->head, prevItem;
	
	if (iterItem->data == data) {
        queue->head = iterItem->next;
        free(iterItem); 
		queue->length--;
        return 0;
    }

	while (iterItem != NULL && iterItem->data != data) {
        prevItem = iterItem;
        iterItem = iterItem->next;
    }

	if (iterItem == NULL){
		return -1;
	}
	prevItem->next = iterItem->next;
	free(iterItem); 
	queue->length--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* https://riptutorial.com/c/example/31818/typedef-for-function-pointers */
	if (!queue || !func ){
		return -1;
	}
	
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