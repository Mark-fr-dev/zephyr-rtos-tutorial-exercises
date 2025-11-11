#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms)*/
#define SLEEPTIME 500
#define SLEEP_COUNT 2
#define LONG_SLEEP 5000

/* Define thread A */
K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

void threadA(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    printk("thread_a: Thread A started\n");
    int i = SLEEP_COUNT;

    while(1) {
        if (i == 0) {
            printk("Sleeping for 5 seconds\n");
            k_sleep(K_MSEC(LONG_SLEEP));
            i = SLEEP_COUNT;
        }
            printk("thread_a: thread loop \n");
        k_msleep(SLEEPTIME);
        i--;
        
    }
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

    k_thread_start(&threadA_data);

    while (1)
    {
        k_msleep(1000);
    }
}
