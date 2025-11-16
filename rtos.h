// rtos.h
#ifndef TINY_RTOS_H
#define TINY_RTOS_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TASKS        8
#define MAX_PRIORITIES   4   // 0 = lowest, MAX_PRIORITIES-1 = highest
#define STACK_WORDS      256 // per-task stack (adjust as needed)

typedef void (*task_func_t)(void *arg);

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED
} task_state_t;

typedef struct tcb_t {
    uint32_t      *sp;                 // current stack pointer
    uint32_t      *stack_base;         // base of allocated stack
    int            priority;
    int            time_slice;         // remaining ticks in this slice
    int            time_slice_reload;  // initial slice count
    task_state_t   state;
    struct tcb_t  *next;              // for ready-queue linked list
} tcb_t;

// Public API
void os_init(void);
int  os_create_task(task_func_t entry, void *arg, int priority);
void os_start(void);

// Called from timer ISR
void os_tick_handler_from_isr(void);

#endif // TINY_RTOS_H
