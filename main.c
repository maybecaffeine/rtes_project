// main.c
#include "rtos.h"

void timer_port_init(void);

void task_blink(void *arg)
{
    (void)arg;
    while (1) {
        for (volatile int i = 0; i < 1000000; ++i); // delay
    }
}

void task_print(void *arg)
{
    (void)arg;
    while (1) {
        for (volatile int i = 0; i < 500000; ++i);
    }
}

int main(void)
{

    os_init();

    os_create_task(task_blink, NULL, 3); 
    os_create_task(task_print, NULL, 2); 

    timer_port_init();   

    os_start();         

    while (1) { }
}
