#ifndef TASKMAIN_H
#define TASKMAIN_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"


/* ========== 任务配置结构体定义 ========== */
typedef struct {
    TaskFunction_t  task_func;          /* 任务函数指针 */
    const char      *task_name;         /* 任务名称 */
    uint16_t        stack_size;         /* 堆栈大小（字节） */
    void            *task_param;        /* 任务参数 */
    UBaseType_t     priority;           /* 任务优先级 */
    TaskHandle_t    *task_handle;       /* 任务句柄指针 */
} TaskConfig_t;


void taskmainInit(void);

#endif