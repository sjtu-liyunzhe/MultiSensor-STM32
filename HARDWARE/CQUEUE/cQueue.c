#include "cQueue.h"

// 链表队列
void initLinkQueue(cLinkQueue* queue)
{
    // queue->front = (node*)malloc(sizeof(node));
    // queue->rear = (node*)malloc(sizeof(node));
    queue->front = NULL;
    queue->rear = NULL;
}

void pushLinkQueue(cLinkQueue* queue, int16_t element)
{
    if (queue->rear == NULL)
    {
        queue->front = queue->rear = (node*)malloc(sizeof(node));
        if (queue->front && queue->rear)
        {
            queue->front->next = NULL;
            queue->rear->next = NULL;
            queue->front->data = queue->rear->data = element;
        }
    }
    else
    {
        queue->rear->next = (node*)malloc(sizeof(node));
        if (queue->rear->next)
        {
            queue->rear->next->data = element;
            queue->rear = queue->rear->next;
            queue->rear->next = NULL;
        }
    }
}

int16_t popLinkQueue(cLinkQueue* queue)
{
    node* tmp = queue->front;
    int16_t popVal = queue->front->data;
    queue->front = queue->front->next;
    if (queue->front == NULL)
        queue->rear = NULL;
    free(tmp);
    return popVal;
}

bool isLinkQueueEmpty(cLinkQueue* queue)
{
    return queue->front == NULL;
}

// 循环队列
void initCircleQueue(cCircleQueue* queue, int size)
{
    queue->queueArray = (int16_t*)malloc(size * sizeof(int16_t));
    queue->maxSize = size;
    queue->front = queue->rear = 0;
}

void doubleCircleQueueSpace(cCircleQueue* queue)
{
    int16_t* tmp = queue->queueArray;
    queue->queueArray = (int16_t*)malloc(2 * queue->maxSize * sizeof(int16_t*));
    if (queue->queueArray)
    {
        for (int i = 1; i < queue->maxSize; i++)
        {
            queue->queueArray[i] = tmp[(queue->front + i) % queue->maxSize];
        }
    }
    queue->front = 0;
    queue->rear = queue->maxSize;
    queue->maxSize *= 2;
    free(tmp);
}

void pushCircleQueue(cCircleQueue* queue, int16_t element)
{
    if ((queue->rear + 1) % queue->maxSize == queue->front)
        doubleCircleQueueSpace(queue);
    queue->rear = (queue->rear + 1) % queue->maxSize;
    queue->queueArray[queue->rear] = element;
}

int16_t popCircleQueue(cCircleQueue* queue)
{
    queue->front = (queue->front + 1) % queue->maxSize;
    return queue->queueArray[queue->front];
}

bool isCircleQueueEmpty(cCircleQueue* queue)
{
    return queue->front == queue->rear;
}
