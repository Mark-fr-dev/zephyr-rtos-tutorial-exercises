#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/mem_manage.h>
#include <string.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define A_PRIORITY 2
#define B_PRIORITY 7

/* delay between greetings (in ms)*/
#define SLEEPTIME 500
#define A_LOOPS 10
#define B_LOOPS 3

/* Define the stack area for thread A */
K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;

void threadA(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    int i;
    for (i = 0; i < A_LOOPS; i++) {
        printk("Hello from Thread A (iteration %d)\n", i + 1);
        k_msleep(SLEEPTIME);
    }
    k_thread_resume(&threadB_data);
    k_thread_suspend(&threadA_data);

}

void threadB(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    int i;
    for (i = 0; i < B_LOOPS; i++) {
        printk("Hello from Thread B (iteration %d)\n", i + 1);
        k_msleep(SLEEPTIME);
    }
    k_thread_resume(&threadA_data);
    k_thread_suspend(&threadB_data);
}

void main(void)
{
    printk("main: Starting Thread A\n");

    k_thread_create(&threadA_data, threadA_stack_area,
                    K_THREAD_STACK_SIZEOF(threadA_stack_area),
                    threadA,
                    NULL, NULL, NULL,
                    A_PRIORITY, 0, K_FOREVER);

    k_thread_name_set(&threadA_data, "thread_a");

    k_thread_create(&threadB_data, threadB_stack_area,
                    K_THREAD_STACK_SIZEOF(threadB_stack_area),
                    threadB,
                    NULL, NULL, NULL,
                    B_PRIORITY, 0, K_FOREVER);

    k_thread_name_set(&threadB_data, "thread_b");

    k_thread_start(&threadB_data);
    k_thread_start(&threadA_data);

    // Optional: keep main thread alive
    while (1)
    {
        k_msleep(1000);
    }
}
