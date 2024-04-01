#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE  100

typedef char* element_t;

typedef struct _node {
    element_t item;
} node;

typedef struct _queue {
    int size;
    int front;
    int rear;
    node *nodes;
} circular_queue;

void create_queue(circular_queue **queue, int size);
void delete_queue(circular_queue *queue);
void enqueue(circular_queue *queue, element_t item);
element_t dequeue(circular_queue *queue);
int get_size(circular_queue *queue);
int is_empty(circular_queue *queue);
int is_full(circular_queue *queue);
#endif