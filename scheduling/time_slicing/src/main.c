/* zephyr-rtos-tutorial - SCHEDULING 
    EXERCISE - Pre-emptive time slicing*/


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define A_PRIORITY 0
#define B_PRIORITY 0
#define C_PRIORITY 0

#define LOOPS 3

/* Define the stack area for thread A */
K_THREAD_STACK_DEFINE(stack_A, STACKSIZE);
static struct k_thread threadA_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(stack_B, STACKSIZE);
static struct k_thread threadB_data;

K_THREAD_STACK_DEFINE(stack_C, STACKSIZE);
static struct k_thread threadC_data;

void threadA(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);
    printk("Thread A: Thread A started.\n");
   
    printk("Thread A: Thread B started by Thread A.\n");

    while (1)
    {
        /* code */
        printk("Thread A: Thread A loop \n");
    }
    

}

void threadB(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);

    printk("Thread B: Thread B started.\n");
    
    while(1)
    {
        /* code */
        printk("Thread B: Thread B loop \n");
    }

}

void threadC(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1); ARG_UNUSED(arg2); ARG_UNUSED(arg3);

    printk("Thread C: Thread C started.\n");

    while (1)   
    {
        /* code */
        printk("Thread C: Thread C loop \n");
    }
    
}

void main(void)
{
    printk("Starting preemptive time slicing demo...\n");

    k_thread_create(&threadA_data, stack_A , STACKSIZE,
                     threadA, NULL, NULL, NULL,
                     A_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&threadA_data, "thread_a");
    
    k_thread_create(&threadB_data, stack_B , STACKSIZE,
                     threadB, NULL, NULL, NULL,
                     B_PRIORITY, 0, K_FOREVER); 
    k_thread_name_set(&threadB_data, "thread_b");

    k_thread_create(&threadC_data, stack_C , STACKSIZE,
                     threadC, NULL, NULL, NULL,
                     C_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&threadC_data, "thread_c");
    
    k_thread_start(&threadA_data);
    k_thread_start(&threadB_data);
    k_thread_start(&threadC_data);
        
    // // Optional: keep main thread alive
    // while (1)
    // {
    //     k_msleep(1000);
    // }
}
