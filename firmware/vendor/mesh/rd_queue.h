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

#define rd_initQueue(...) GET_MACRO(__VA_ARGS__, rd_initQueue2, rd_initQueue1)(__VA_ARGS__)
#define GET_MACRO(_1, _2,_3,_4,_5, NAME, ...) NAME

int rd_initQueue1(rd_queue_t *q, int maxSize, int elementSize, void *dataArray);
int rd_initQueue2(rd_queue_t *q, int maxSize, int elementSize, void *dataArray,rd_func_handle_t func_handle);

int rd_enqueue(rd_queue_t *q, void *element);
int rd_dequeue(rd_queue_t *q, void *element);
int isQueueEmpty(rd_queue_t *q);

int get_number_func_loop();
int rd_register_func_loop(rd_func_handle_t func_handle);

void rd_handle_queue_loop();

#endif
