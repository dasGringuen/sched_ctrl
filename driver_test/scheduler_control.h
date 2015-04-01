/**
 *   @file scheduler_control.h
 *   @brief -
 *
 *   @author Adrian Remonda
 *
 */
#ifndef SCHEDULER_CONTROL_
#define SCHEDULER_CONTROL_

#define SWITCH_PATH "/sys/kernel/sch/switch_to"
#define WAIT_PATH "/sys/kernel/sch/wait_for_control"
#define DEBUG_PATH "/sys/kernel/sch/debug_sched"
#define RESET_PATH "/sys/kernel/sch/reset"
#define WAKE_ALL_PATH "/sys/kernel/sch/wake_all"

int sched_show_tasks(void);

/*
 * Waits until the scheduler context-switches to the calling thread;
   once the calling thread gets control, context switches are disabled
   until a switch_to() is called. IOW: context switch is handled manually.
   */
int wait_for_control(int master_thread);
int switch_to(int t);

void start_test(void);
int finish_test(void);

/* TODO not used */
int reset_driver(void);

#endif /* SCHEDULER_CONTROL_ */
