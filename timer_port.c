// timer_port.c
#include "rtos.h"

static void timer_isr(void)
{

    os_tick_handler_from_isr();
}

void timer_port_init(void)
{
    timer_init(1000, timer_isr); 
    timer_start();
}
