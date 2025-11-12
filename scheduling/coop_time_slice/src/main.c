#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define A_PRIORITY -2
#define B_PRIORITY -1

#define A_LOOPS 10
#define B_LOOPS 3

/* Define the stack area for thread A */
K_THREAD_STACK_DEFINE(stack_A, STACKSIZE);
// static struct k_thread threadA_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(stack_B, STACKSIZE);
// static struct k_thread threadB_data;

void threadA(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);

    for (int i = 0; i < A_LOOPS; i++) {
        printk("Hello from Thread A (iteration %d)\n", i + 1);
    
    }
    printk("Thread A completed all iterations.\n");

}

void threadB(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);

    for (int i = 0; i < B_LOOPS; i++) {
        printk("Hello from Thread B (iteration %d)\n", i + 1);
        k_yield();  
    }
    printk("Thread B completed all iterations.\n");
}

void main(void)
{
    printk("Starting cooperative time slicing demo...\n");

    k_thread_create(NULL, stack_A, STACKSIZE,
                     threadA, NULL, NULL, NULL,
                     A_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(NULL, "thread_a");
    
    k_thread_create(NULL, stack_B , STACKSIZE,
                     threadB, NULL, NULL, NULL,
                     B_PRIORITY, 0, K_FOREVER); 

    k_thread_start(NULL);

    // // Optional: keep main thread alive
    // while (1)
    // {
    //     k_msleep(1000);
    // }
}
