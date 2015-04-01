#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/syscall.h>

#include "scheduler_control.h"

int t_pid[32];
pthread_t t1;
pthread_t t2;
pthread_t t3;

void print_all_tasks(void)
{
    printf("Father:%d\n", t_pid[0]);
    printf("t1: pid %d\n", t_pid[1]);
    printf("t2: pid %d\n", t_pid[2]);
    printf("t3: pid %d\n", t_pid[3]);

    /* for debugging the driver */
    sched_show_tasks();
}

/*//////////////////////////////////////////////////////*/

static void* t1_f(void* arg)
{
    int i, ret;
    (void)arg;

    /* must get the pid first */
    t_pid[1] = syscall(SYS_gettid);

    /* for syncronization of the tasks */
    ret = wait_for_control(1);
    if (ret)
        return NULL;

    /* start the test */
    start_test();

    /* for debugging */
    print_all_tasks();

    for (i = 0; i < 10; i++)
        printf("T1_1");
    printf("\n");

    switch_to(t_pid[2]);

    for (i = 0; i < 10; i++)
        printf("T1_2");
    printf("\n");

    /* wake the other threads and leave */
    finish_test();

    return NULL;
}

static void* t2_f(void* arg)
{
    int i, ret;
    (void)arg;

    /* must get the pid first */
    t_pid[2] = syscall(SYS_gettid);

    ret = wait_for_control(0);
    if (ret)
        return NULL;

    for (i = 0; i < 10; i++)
        printf("T2");
    printf("\n");

    switch_to(t_pid[3]);
    return NULL;
}

static void* t3_f(void* arg)
{
    int i, ret;
    (void)arg;

    /* must get the pid first */
    t_pid[3] = syscall(SYS_gettid);

    ret = wait_for_control(0);
    if (ret)
        return NULL;

    for (i = 0; i < 10; i++)
        printf("T3");
    printf("T3\n");

    switch_to(t_pid[1]);
    return NULL;
}

int main(void)
{
    t_pid[0] = syscall(SYS_gettid);

    reset_driver();

    pthread_create(&t1, NULL, t1_f, NULL);
    pthread_create(&t2, NULL, t2_f, NULL);
    pthread_create(&t3, NULL, t3_f, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    return 0;
}

/* Expected output always:
    t1 - 1
    ... (9 more times) ...
    t2
    ... (9 more times) ...
    t1 - 2
    ... (9 more times) ...
*/

