#ifndef RD_QUEUE_H
#define RD_QUEUE_H

#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "RD_log/RD_log.h"

typedef struct {
    void *data;
    int front;
    int rear;
    int maxSize;
    int elementSize;
} rd_queue_t;

typedef void (*rd_func_handle_t)(void);

//void rd_initQueue(rd_queue_t *q, int maxSize, int elementSize, void *dataArray);
int rd_initQueue(rd_queue_t *q, int maxSize, int elementSize, void *dataArray,rd_func_handle_t func_handle);
int rd_enqueue(rd_queue_t *q, void *element);
int rd_dequeue(rd_queue_t *q, void *element);
int isQueueEmpty(rd_queue_t *q);

void rd_handle_queue_loop();
#endif
