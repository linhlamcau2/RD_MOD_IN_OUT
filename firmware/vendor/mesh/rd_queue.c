#include "rd_queue.h"

#define MAX_POSSIBLE_FUNCS 20


static rd_func_handle_t  rd_func_handle[MAX_POSSIBLE_FUNCS];
static int rd_func_handle_count = 0;

int is_full_register()
{
	return (rd_func_handle_count == MAX_POSSIBLE_FUNCS -1);
}

int rd_initQueue(rd_queue_t *q, int maxSize, int elementSize, void *dataArray,rd_func_handle_t func_handle)
{
	if(is_full_register())
		return -1;
    q->data = dataArray;
    q->front = -1;
    q->rear = 0;
    q->maxSize = maxSize;
    q->elementSize = elementSize;
    rd_func_handle[rd_func_handle_count ++] = func_handle;
    return 0;
}

int get_num_queue()
{
	return MAX_POSSIBLE_FUNCS -rd_func_handle_count;
}

void rd_handle_queue_loop()
{
	for (int i = 0; i < rd_func_handle_count; i++)
	{
		if (rd_func_handle[i]) {
			rd_func_handle[i]();
		}
	}
}
int isQueueEmpty(rd_queue_t *q)
{
    return (q->front == -1);
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
    if (isQueueEmpty(q))
    {
        q->rear = 0;
        q->front = 0;
    }
    else
    {
        q->rear = (q->rear + 1) % q->maxSize;
    }
    memcpy((void *)((char*)q->data + q->rear * q->elementSize), element, q->elementSize);
//    RD_ev_log("enqueue rear: %d,front: %d\n",q->rear,q->front);
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
        q->front = -1;
    }
    else
    {
        q->front = (q->front + 1) % (q->maxSize);
    }
    return 0;
}


