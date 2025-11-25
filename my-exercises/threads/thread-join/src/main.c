#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms)*/
#define SLEEPTIME 500

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;


void threadA(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    printk("thread_a: Thread A started\n");

    printk("thread_a: waiting for thread_b to complete \n");
    k_thread_join(&threadB_data, K_FOREVER); // wait forever until thread_b returns

    while (1) {
        printk("thread_a: thread loop \n");
        k_msleep(SLEEPTIME);
    }
}

void threadB(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    printk("thread_b: thread started \n");

    k_msleep(SLEEPTIME);

    printk("thread_b: returning to thread_a \n");
}

void main(void)
{
    printk("main: Starting Thread A\n");

    k_thread_create(&threadA_data, threadA_stack_area,
                    K_THREAD_STACK_SIZEOF(threadA_stack_area),
                    threadA,
                    NULL, NULL, NULL,
                    PRIORITY, 0, K_FOREVER);
    
    k_thread_name_set(&threadA_data, "thread_a");

    k_thread_create(&threadB_data, threadB_stack_area,
                    K_THREAD_STACK_SIZEOF(threadB_stack_area),
                    threadB,
                    NULL, NULL, NULL,
                    PRIORITY, 0, K_FOREVER);
   
    k_thread_name_set(&threadB_data, "thread_b");

    k_thread_start(&threadA_data);
    k_thread_start(&threadB_data);
}
