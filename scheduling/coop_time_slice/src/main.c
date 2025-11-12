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
static struct k_thread threadA_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(stack_B, STACKSIZE);
static struct k_thread threadB_data;

void threadA(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);
    printk("Thread A: Thread A started.\n");
    k_thread_start(&threadB_data);
    printk("Thread A: Thread B started by Thread A.\n");

    for (int i = 0; i < A_LOOPS; i++) {
        printk("Thread A: Thread A (iteration %d)\n", i + 1);
    
    }
    printk("Thread A: completed all iterations, aborted.\n");
    k_thread_abort(&threadA_data);

}

void threadB(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);

    printk("Thread B: Thread B started.\n");

    for (int i = 0; i < B_LOOPS; i++) {
        printk("Thread B: Thread B (iteration %d)\n", i + 1);
        k_yield();  
    }
    printk("Thread B: completed all iterations, aborted.\n");
    k_thread_abort(&threadB_data);
}

void main(void)
{
    printk("Starting cooperative time slicing demo...\n");

    k_thread_create(&threadA_data, stack_A , STACKSIZE,
                     threadA, NULL, NULL, NULL,
                     A_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&threadA_data, "thread_a");
    
    k_thread_create(&threadB_data, stack_B , STACKSIZE,
                     threadB, NULL, NULL, NULL,
                     B_PRIORITY, 0, K_FOREVER); 
    k_thread_name_set(&threadB_data, "thread_b");

    k_thread_start(&threadA_data);
    
    
    // // Optional: keep main thread alive
    // while (1)
    // {
    //     k_msleep(1000);
    // }
}
