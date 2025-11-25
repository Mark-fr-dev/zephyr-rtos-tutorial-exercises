#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms)*/
#define SLEEPTIME 500
#define A_LOOPS 3
#define B_LOOPS 4

/* Define the stack area for thread A */
K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;

void threadA(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    k_thread_suspend(&threadA_data);

    printk("thread_a: Thread A started\n");
    
    while(1){
    for (int i = 0; i < A_LOOPS; i++) {
         printk("thread_a: thread loop %d \n", i);
         k_msleep(SLEEPTIME);
    }
    printk("thread_a: yielding to thread B\n");
    k_thread_resume(&threadB_data);
    k_thread_suspend(&threadA_data);
}
        
}

void threadB(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    printk("thread_b: Thread B started\n");
    
    while(1) {
        for(int i = 0; i < B_LOOPS; i++) {
            printk("thread_b: thread loop %d \n", i);
            k_msleep(SLEEPTIME);
        }
        printk("thread_b: Resuming thread A\n");
        k_thread_resume(&threadA_data);
        k_thread_suspend(&threadB_data);
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

    k_thread_create(&threadB_data, threadB_stack_area,
                    K_THREAD_STACK_SIZEOF(threadB_stack_area),
                    threadB,
                    NULL, NULL, NULL,
                    PRIORITY, 0, K_FOREVER);

    k_thread_name_set(&threadB_data, "thread_b");

    k_thread_start(&threadB_data);
    k_thread_start(&threadA_data);

    // Optional: keep main thread alive
    while (1)
    {
        k_msleep(1000);
    }
}
