#ifndef _CQUEUE_H_
#define _CQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
}cLinkQueue;

void InitLinkQueue(cLinkQueue* queue);
void pushLinkQueue(cLinkQueue* queue, int16_t element);
int16_t popLinkQueue(cLinkQueue* queue);
bool isLinkQueueEmpty(cLinkQueue* queue);

// 循环队列
typedef struct cCircleQueue
{
    int16_t* queueArray;
    int maxSize;
    int front, rear;
}cCircleQueue;

void initCircleQueue(cCircleQueue* queue, int size);
void doubleCircleQueueSpace(cCircleQueue* queue);
void pushCircleQueue(cCircleQueue* queue, int16_t element);
int16_t popCircleQueue(cCircleQueue* queue);
bool isCircleQueueEmpty(cCircleQueue* queue);

#endif // !CQUEUE_H
