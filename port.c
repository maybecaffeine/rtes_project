// port.c
#include "rtos.h"

extern void vPortStartFirstTask(uint32_t *sp);
extern void vPortSwitch(uint32_t *old_sp, uint32_t *new_sp);

uint32_t *port_init_stack(uint32_t *top, task_func_t entry, void *arg)
{
    uint32_t *sp = top;

    *(--sp) = (uint32_t)arg;    // R0
    *(--sp) = 0;                // R1
    *(--sp) = 0;                // R2
    *(--sp) = 0;                // R3
    *(--sp) = 0;                // R4
    *(--sp) = 0;                // R5
    *(--sp) = 0;                // R6
    *(--sp) = 0;                // R7
    *(--sp) = 0;                // R8
    *(--sp) = 0;                // R9
    *(--sp) = 0;                // R10
    *(--sp) = 0;                // R11
    *(--sp) = 0;                // R12
    *(--sp) = (uint32_t)entry;  // LR 
    *(--sp) = (uint32_t)entry;  // PC
    *(--sp) = 0x60000010;       // CPSR 

    return sp;
}

void port_start_first_task(tcb_t *first)
{
    vPortStartFirstTask(first->sp);
}

void port_switch_to(tcb_t *next)
{
    extern tcb_t *current_tcb;
    vPortSwitch(&current_tcb->sp, next->sp);
}
