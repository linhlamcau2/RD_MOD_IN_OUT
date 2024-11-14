#include "rd_queue.h"

void rd_initQueue(rd_queue_t *q, int maxSize, int elementSize, void *dataArray)
{
    q->data = dataArray;
    q->front = 0;
    q->rear = -1;
    q->maxSize = maxSize;
    q->elementSize = elementSize;
}

int isQueueEmpty(rd_queue_t *q)
{
    return (q->rear == -1);
}

int isQueueFull(rd_queue_t *q)
{
    return (((q->rear + 1) % (q->maxSize)) == q->front);
}

int rd_enqueue(rd_queue_t *q, void *element)
{
    if (isQueueFull(q))
    {
        return -1;
    }
    if (q->rear == -1)
    {
        q->rear = 0;
        q->front = 0;
    }
    else
    {
        q->rear = (q->rear + 1) % q->maxSize;
    }
    memcpy((void *)((char*)q->data + q->rear * q->elementSize), element, q->elementSize);
    return 0;
}

int rd_dequeue(rd_queue_t *q, void *element)
{
    if (isQueueEmpty(q))
    {
        return -1;
    }
    memcpy(element, (void *)((char*)q->data + q->front * q->elementSize), q->elementSize);

    if (q->front == q->rear)
    {
        q->rear = -1;
    }
    else
    {
        q->front = (q->front + 1) % (q->maxSize);
    }
    return 0;
}


