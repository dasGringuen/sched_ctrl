/**
 *   @file scheduler_control.c
 *   @brief -
 *
 *   @author Adrian Remonda
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/syscall.h>

#include "scheduler_control.h"

int g_fd;

int open_device(char* dev)
{
    int fd; /* File description */

    if (dev == NULL) {
        return -1;
    }

    fd = open(dev, O_RDWR);
    if (fd < 0) {
        printf("No such file or directory: %s\n", dev);
        return -1;
    }

    return fd;
}

int close_device(int fd)
{
    int ret;

    ret = close(fd);
    if (ret < 0) {
        perror(__FUNCTION__);
        return ret;
    }
    return 0;
}

int write_device(int fd, char *val, unsigned int len)
{
    int ret;

    if (val == NULL) {
        return -1;
    }

    /* Send commands to FreeRTOS */
    ret = write(fd, val, len);
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }
    return ret;
}

int read_device(int fd, char *val, unsigned int max_len)
{
    int ret;

    ret = read(fd, val, max_len);
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    printf("%s\n", val);
    return 0;
}

int sched_show_tasks(void)
{
    int fd;
    int t = 0;
    int ret;
    char val[32] = "";
    char comm[32] = "";

    sprintf(val, "%.1u", t);

    fd = open_device(DEBUG_PATH);
    if (fd < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    ret =  write_device(fd, val, sizeof(val));
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    printf("%s\n", comm);
    close(fd);
    return 0;
}

int reset_driver(void)
{
    int fd;
    int ret;
    char comm[32] = "";

    fd = open_device(RESET_PATH);
    if (fd < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    ret = read(fd, comm, sizeof(comm));
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    /* printf("%s\n", comm); */
    close(fd);
    return 0;
}

void start_test(void)
{
    printf("Starting test... \n");

    /* just in case sleep a bit, to have all the threas pid */
    nanosleep(100 * 1000);
}

int finish_test(void)
{
    int fd;
    int ret;
    char comm[32] = "";

    fd = open(WAKE_ALL_PATH, O_RDONLY);
    if (fd < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    ret = read(fd, comm, sizeof(comm));
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    /* printf("%s\n", comm); */
    close(fd);
    return 0;
}

int wait_for_control(int master_thread)
{
    int fd;
    char val[32] = "";
    int ret;

    fd = open_device(WAIT_PATH);
    if (fd < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    if (master_thread)
        strcpy(val, "1");
    else
        strcpy(val, "0");

    ret =  write_device(fd, val, sizeof(val));
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    /* printf("%s\n", comm); */
    close(fd);
    return 0;
}

int switch_to(int t)
{
    (void)t;
    int fd;
    int ret;
    char val[32] = "";

    sprintf(val, "%.1u", t);

    fd = open_device(SWITCH_PATH);
    if (fd < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    ret =  write_device(fd, val, sizeof(val));
    if (ret < 0) {
        perror(__FUNCTION__);
        return -1;
    }

    /* printf("%s\n", comm); */
    close(fd);
    return 0;
}
