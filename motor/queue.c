#include "queue.h"
#include "server.h"

void create_queue(circular_queue **queue, int size) {
    (*queue) = (circular_queue *)malloc(sizeof(circular_queue));

    (*queue)->nodes = (node *)malloc(sizeof(node) * size + 1);

    (*queue)->size = size;
    (*queue)->front = 0;
    (*queue)->rear = 0;
}

void delete_queue(circular_queue *queue) {
    free(queue->nodes);
    free(queue);
}

void enqueue(circular_queue *queue, element_t item) {
    int pos = 0;

    while(is_full(queue));

    if(queue->rear == queue->size) {
        pos = queue->rear;
        queue->rear = 0;
    } else {
        pos = queue->rear++;
    }

    queue->nodes[pos].item = item;
    fprintf(stderr, "\"%s\"\t is enqueued at %d\n", item, pos);

    #if DEBUG
        fprintf(stderr, "\"%s\"\t is enqueued at %d\n", item, pos);
    #endif
}

element_t dequeue(circular_queue *queue) {
    int pos = queue->front;

    while(is_empty(queue));

    if(queue->front == queue->size) {
        queue->front = 0;
    } else {
        queue->front++;
    }

    element_t dequeued_item = queue->nodes[pos].item;

    #if DEBUG
        fprintf(stderr, "\"%s\"\t is dequeued from %d\n", dequeued_item, pos);
    #endif

    return dequeued_item;
}
int get_size(circular_queue *queue) {
    if(queue->front <= queue->rear) {
        return (queue->rear - queue->front);
    } else {
        return (queue->rear + (queue->size - queue->front) + 1);
    }
}

int is_empty(circular_queue *queue) {
    return (queue->front == queue->rear);
}

int is_full(circular_queue *queue) {
    if(queue->front < queue->rear) {
        return ((queue->rear - queue->front) == queue->size);
    } else {
        return ((queue->rear + 1) == queue->front);
    }
}
