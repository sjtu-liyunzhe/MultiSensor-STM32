#include "cQueue.h"
#include <stdlib.h>
#include <assert.h>

#define CIRCLE_SIZE 50
// 链表队列
void initLinkQueue(cLinkQueue* queue)
{
    // queue->front = (node*)malloc(sizeof(node));
    // queue->rear = (node*)malloc(sizeof(node));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

bool pushLinkQueue(cLinkQueue* queue, int16_t element)
{
    if (queue->rear == NULL)
    {
        // queue->front = queue->rear = (node*)malloc(sizeof(node));
        queue->front = queue->rear = (node*)pvPortMalloc(sizeof(node));
        // assert(queue->front && queue->rear);
        if (queue->front && queue->rear)
        {
            queue->front->next = NULL;
            queue->rear->next = NULL;
            queue->front->data = queue->rear->data = element;
            queue->size++;
        }
        else
            return false;
        // else
        // {
        //     fprintf(stderr, "falied to malloc");
        //     exit(-1);
        // }
    }
    else
    {
        queue->rear->next = (node*)pvPortMalloc(sizeof(node));
        // assert(queue->rear->next);
        if (queue->rear->next)
        {
            queue->rear->next->data = element;
            queue->rear = queue->rear->next;
            queue->rear->next = NULL;
            queue->size++;
        }
        else
            return false;
    }
    return true;
    
}

int16_t popLinkQueue(cLinkQueue* queue)
{
    node* tmp = queue->front;
    int16_t popVal = queue->front->data;
    queue->front = queue->front->next;
    if (queue->front == NULL)
        queue->rear = NULL;
    vPortFree(tmp);
    queue->size--;
    return popVal;
}

bool isLinkQueueEmpty(cLinkQueue* queue)
{
    // return queue->front->next == queue->rear;
    return queue->front == NULL;
}

// 循环队列
bool initCircleQueue(cCircleQueue* queue, int size)
{
    queue->queueArray = (int16_t*)pvPortMalloc(size * sizeof(int16_t));
    queue->size = 0;
    // assert(queue->queueArray);
    if(queue->queueArray)
    {
        queue->maxSize = size;
        queue->front = queue->rear = 0;
    }
    else
        return false;
    return true;
}

bool doubleCircleQueueSpace(cCircleQueue* queue)
{
    int16_t* tmp = queue->queueArray;
    queue->queueArray = (int16_t*)pvPortMalloc(2 * queue->maxSize * sizeof(int16_t*));
    if (queue->queueArray)
    {
        for (int i = 1; i < queue->maxSize; i++)
        {
            queue->queueArray[i] = tmp[(queue->front + i) % queue->maxSize];
        }
    }
    else
        return false;
    queue->front = 0;
    queue->rear = queue->maxSize;
    queue->maxSize *= 2;
    vPortFree(tmp);
    return true;
}

bool pushCircleQueue(cCircleQueue* queue, int16_t element)
{
    bool flag = true;
    if ((queue->rear + 1) % queue->maxSize == queue->front)
        flag = doubleCircleQueueSpace(queue);
    queue->rear = (queue->rear + 1) % queue->maxSize;
    queue->queueArray[queue->rear] = element;
    ++(queue->size);
    return flag;
}

int16_t popCircleQueue(cCircleQueue* queue)
{
    queue->front = (queue->front + 1) % queue->maxSize;
    --(queue->size);
    return queue->queueArray[queue->front];
}

bool isCircleQueueEmpty(cCircleQueue* queue)
{
    return queue->front == queue->rear;
}

bool initIMULinkQueue(IMULinkQueue* imuLinkQueue)
{
    imuLinkQueue->rowQueue = (cLinkQueue*)pvPortMalloc(sizeof(cLinkQueue));
    imuLinkQueue->pitchQueue = (cLinkQueue*)pvPortMalloc(sizeof(cLinkQueue));
    imuLinkQueue->yawQueue = (cLinkQueue*)pvPortMalloc(sizeof(cLinkQueue));
    if(imuLinkQueue->rowQueue && imuLinkQueue->pitchQueue && imuLinkQueue->yawQueue)
    {
        initLinkQueue(imuLinkQueue->rowQueue);
        initLinkQueue(imuLinkQueue->pitchQueue);
        initLinkQueue(imuLinkQueue->yawQueue);
    }
    else
        return false;
    return true;
}

bool initIMUCircleQueue(IMUCircleQueue* imuCircleQueue, u8 size)
{
    imuCircleQueue->rowQueue = (cCircleQueue*)pvPortMalloc(sizeof(cCircleQueue));
    imuCircleQueue->pitchQueue = (cCircleQueue*)pvPortMalloc(sizeof(cCircleQueue));
    imuCircleQueue->yawQueue = (cCircleQueue*)pvPortMalloc(sizeof(cCircleQueue));
    if(imuCircleQueue->rowQueue && imuCircleQueue->pitchQueue && imuCircleQueue->yawQueue)
    {
        initCircleQueue(imuCircleQueue->rowQueue, size);
        initCircleQueue(imuCircleQueue->pitchQueue, size);
        initCircleQueue(imuCircleQueue->yawQueue, size);
    }
    else
        return false;
    return true;
}