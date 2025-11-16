// rtos.c
#include "rtos.h"
static tcb_t tcb_array[MAX_TASKS];
static uint32_t task_stacks[MAX_TASKS][STACK_WORDS];

static tcb_t *ready_queues[MAX_PRIORITIES]; 
static tcb_t *current_tcb = NULL;
static int    task_count = 0;

extern void port_start_first_task(tcb_t *first);
extern void port_switch_to(tcb_t *next);

static void enqueue_task(tcb_t **queue_head, tcb_t *t)
{
    if (*queue_head == NULL) {
        t->next = t;      
        *queue_head = t;
    } else {
        tcb_t *head = *queue_head;
        tcb_t *tail = head;
        while (tail->next != head) {
            tail = tail->next;
        }
        tail->next = t;
        t->next = head;
    }
}

static void remove_task(tcb_t **queue_head, tcb_t *t)
{
    if (*queue_head == NULL) return;

    tcb_t *head = *queue_head;
    tcb_t *prev = head;
    tcb_t *cur  = head;

    do {
        if (cur == t) break;
        prev = cur;
        cur  = cur->next;
    } while (cur != head);

    if (cur != t) return; 

    if (cur == head && cur->next == head) {
        *queue_head = NULL;
    } else {
        prev->next = cur->next;
        if (cur == head) {
            *queue_head = cur->next;
        }
    }
}

// api

void os_init(void)
{
    task_count = 0;
    current_tcb = NULL;
    for (int i = 0; i < MAX_PRIORITIES; ++i) {
        ready_queues[i] = NULL;
    }
}

int os_create_task(task_func_t entry, void *arg, int priority)
{
    if (task_count >= MAX_TASKS) return -1;
    if (priority < 0 || priority >= MAX_PRIORITIES) return -2;

    tcb_t *t = &tcb_array[task_count];
    uint32_t *stack_top = &task_stacks[task_count][STACK_WORDS];

    extern uint32_t *port_init_stack(uint32_t *top, task_func_t entry, void *arg);

    t->stack_base       = &task_stacks[task_count][0];
    t->sp               = port_init_stack(stack_top, entry, arg);
    t->priority         = priority;
    t->time_slice       = 5;   
    t->time_slice_reload= 5;
    t->state            = TASK_READY;
    t->next             = NULL;

    enqueue_task(&ready_queues[priority], t);
    task_count++;

    return task_count - 1; 
}


static tcb_t *scheduler_pick_next(void)
{
    for (int prio = MAX_PRIORITIES - 1; prio >= 0; --prio) {
        tcb_t *queue = ready_queues[prio];
        if (queue != NULL) {
            // roundrobin
            ready_queues[prio] = queue->next;
            return ready_queues[prio]; 
        }
    }
    return NULL; 
}

void os_start(void)
{
    
    current_tcb = scheduler_pick_next();
    if (!current_tcb) {
        while (1) { }
    }
    current_tcb->state = TASK_RUNNING;
    port_start_first_task(current_tcb);
}


void os_tick_handler_from_isr(void)
{
    if (!current_tcb) return;

    current_tcb->time_slice--;
    if (current_tcb->time_slice <= 0) {
        
        current_tcb->time_slice = current_tcb->time_slice_reload;
        current_tcb->state      = TASK_READY;

        enqueue_task(&ready_queues[current_tcb->priority], current_tcb);
       
        tcb_t *next = scheduler_pick_next();
        if (next && next != current_tcb) {
            tcb_t *prev = current_tcb;
            current_tcb = next;
            current_tcb->state = TASK_RUNNING;
            port_switch_to(current_tcb);
            
        }
    }
    
}
