/* EXERCISE: Practice using interupts*/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h> /* size of stack area used by each thread */
#include <zephyr/irq.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/sem.h>
#include <zephyr/drivers/gpio.h>

#define STACKSIZE 1024 /* scheduling priority used by each thread */
#define PRIORITY 7
#define SLEEPTIME 500 /* Define the stack area for thread A */

K_SEM_DEFINE(button_pressed_sem, 0, 1); // starts at 0 = no press yet
K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data; /* Devicetree node identifier for "sw0" alias*/

/* nucleo_wb55rg has sw0 but it's disabled by default → force it okay */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#undef DT_NODE_HAS_STATUS_OKAY
#define DT_NODE_HAS_STATUS_OKAY(node_id) 1
#endif

/* Correct and simple way for nucleo_wb55rg */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_callback button_cb_data;


/* This function runs when the button interrupt fires */
/* Your direct ISR — note the special signature for GPIO callbacks */
// ISR_DIRECT_DECLARE(button_isr)
// {
//     gpio_pin_interrupt_configure_dt(&button, GPIO_INT_DISABLE);

//     printk("Button pressed! (from direct ISR)\n");

//     /* This tells threadA: "stop looping now" */
//     k_sem_give(&button_pressed_sem);

//     /* Re-arm interrupt for next press */
//     gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);

//     ISR_DIRECT_PM();
//     return 1;
// }

void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins)
{
    printk("Button pressed!\n");
    k_sem_give(&button_pressed_sem);
    return;
}

void button_setup(void)
{
    if (!device_is_ready(button.port))
    {
        printk("Error: button GPIO device not ready!\n");
        return;
    }

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
}

void threadA(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);
    int i = 0;
    printk("thread_a: Thread A started\n");
    while (1)
    {
        printk("thread_a: thread loop %d \n", i);
        i++;

        /* Check if button was pressed — non-blocking! */
        if (k_sem_take(&button_pressed_sem, K_NO_WAIT) == 0)
        {
            printk("thread_a: BUTTON DETECTED! Stopping loop.\n");
            i = 0; // reset counter
            k_msleep(100);
            //break; // ← exits the loop → thread ends or can continue
        }

        k_msleep(SLEEPTIME);
    }

    printk("thread_a: Loop stopped. Thread is now idle or can do other work.\n");

    /* Optional: loop forever here so thread doesn't exit */
    while (1)
        k_msleep(1000);
}
void main(void)
{
    button_setup();
    k_thread_create(&threadA_data, threadA_stack_area, STACKSIZE,
                    threadA, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
    //k_thread_start(&threadA_data);

}