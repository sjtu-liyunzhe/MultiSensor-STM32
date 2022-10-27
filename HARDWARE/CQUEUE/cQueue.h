#ifndef _CQUEUE_H_
#define _CQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"

// 链表队列
typedef struct node
{
    int16_t data;
    struct node* next;
}node, * nodePtr;
typedef struct cLinkQueue
{
    node* front;    // 队头指针
    node* rear;     // 队尾指针
    int size;
}cLinkQueue;

void initLinkQueue(cLinkQueue* queue);
bool pushLinkQueue(cLinkQueue* queue, int16_t element);
int16_t popLinkQueue(cLinkQueue* queue);
bool isLinkQueueEmpty(cLinkQueue* queue);

// 循环队列
typedef struct cCircleQueue
{
    int16_t* queueArray;
    int maxSize;
    int front, rear;
    int size;
}cCircleQueue;

bool initCircleQueue(cCircleQueue* queue, int size);
bool doubleCircleQueueSpace(cCircleQueue* queue);
bool pushCircleQueue(cCircleQueue* queue, int16_t element);
int16_t popCircleQueue(cCircleQueue* queue);
bool isCircleQueueEmpty(cCircleQueue* queue);

// IMU数据封装
typedef struct IMULinkQueue
{
    cLinkQueue* rowQueue;
    cLinkQueue* pitchQueue;
    cLinkQueue* yawQueue;
}IMULinkQueue;

typedef struct IMUCircleQueue
{
    cCircleQueue* rowQueue;
    cCircleQueue* pitchQueue;
    cCircleQueue* yawQueue;
}IMUCircleQueue;


bool initIMULinkQueue(IMULinkQueue* imuLinkQueue);
bool initIMUCircleQueue(IMUCircleQueue* imuCircleQueue, u8 size);

#endif // !CQUEUE_H
