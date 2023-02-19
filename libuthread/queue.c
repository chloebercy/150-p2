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
	item_p head;
	item_p tail;
	int length;
};

queue_t queue_create(void)
{
	// Initialize new pointer to struct queue
	queue_t newQueuePtr = (queue_t) malloc(sizeof(struct queue));

	// Error Handling
	if (newQueuePtr == NULL){
		return NULL;
	}

	// Define member variables of struct instance accordingly
	newQueuePtr->head = NULL;
	newQueuePtr->head = NULL;
	newQueuePtr->length = 0;

	return newQueuePtr;
}

int queue_destroy(queue_t queue)
{
	// Error Handling
	if (!queue || queue->length > 0){
		return -1;
	}

	// Free memory pointed to by queue
	free(queue);
	queue = NULL;

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	// Error Handling
	if (!queue || !data){
		return -1; 
	}

	// Initalize new pointer to struct item + Error Handling
	item_p newItemPtr = (item_p) malloc(sizeof(struct item));
	if (!newItemPtr){
		return -1;
	}
	
	// Define member variables of struct instance
	newItemPtr->data = data;
	newItemPtr->next = NULL;

	// Add newItemPtr to end of queue
	if (queue->head == NULL){
		queue->head = newItemPtr;
		queue->tail = newItemPtr;
	} 
	else {
		queue->tail->next = newItemPtr;
		queue->tail = newItemPtr;
	}
	
	// Increase queue length
	queue->length++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	// Error Handling
	if (!queue || !data || !queue->head){
		return -1;
	}

	// Gets first item and its data from queue, pointed to by 'head'
	item_p oldestItem = queue->head;
	*data = oldestItem->data;
	queue->head = oldestItem->next;
	free(oldestItem);

	// Decrease queue length
	queue->length--;

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	// Error Handling
	if (!queue || !data ){
		return -1;
	}

	// Temporary item_p variables for iteration
	item_p iterItem = queue->head, prevItem;
	
	// Specific case if head of queue is target data item
	if (iterItem->data == data) {
        queue->head = iterItem->next;
        free(iterItem); 
		queue->length--;
        return 0;
    }

	// Iterate through queue looking for target data item
	while (iterItem != NULL && iterItem->data != data) {
        prevItem = iterItem;
        iterItem = iterItem->next;
    }

	// Target data item not found
	if (iterItem == NULL) {
		return -1;
	}

	// Targe data item found
	prevItem->next = iterItem->next;
	free(iterItem); 
	queue->length--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	// Error Handling
	if (!queue || !func ){
		return -1;
	}
	
	// Iterate through queue and call function func
	item_p iterItem;
	for (iterItem = queue->head; iterItem != NULL; iterItem = iterItem->next){
		func(queue, iterItem->data);
	}
	iterItem = NULL;

	return 0;
}

int queue_length(queue_t queue)
{
	// Error Handling
	if (!queue){
		return -1;
	}

	return queue->length;
}