/* EXERCISE: Button press wakes a sleeping thread using a direct ISR */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/sem.h> 

/* Stack and priority */
#define STACKSIZE 1024
#define THREAD_PRIO 7

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

/* How long thread sleeps when there is no button press */
#define SLEEPTIME_MS 500

/* Use the correct alias for Nucleo-WB55RG → sw0 */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

/* Make sure the board really defines sw0 (it does on nucleo_wb55rg) */
#if !DT_NODE_HAS_STATUS(DT_ALIAS(sw0), okay)
#error "Unsupported board: sw0 devicetree alias is not defined or disabled"
#endif

/* Semaphore that the ISR gives and the thread takes */
//K_SEMAPHORE_DEFINE(button_sem);
K_SEMAPHORE_DEFINE(button_sem, 0, 1);

/* ------------------------------------------------------------------ */
/* Direct (zero-latency) ISR                                          */
/* ------------------------------------------------------------------ */
ISR_DIRECT_DECLARE(button_isr)
{
    /* Clear pending interrupt on the GPIO controller */
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_DISABLE);

    printk("Button pressed! (direct ISR)\n");

    /* Wake the waiting thread */
    k_sem_give(&button_sem);

    /* Re-enable the interrupt (edge-triggered) */
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);

    ISR_DIRECT_PM(); /* Power management at the right point */
    return 1;        /* "reschedule may be needed" */
}

/* ------------------------------------------------------------------ */
/* Thread A — sleeps until button is pressed                          */
/* ------------------------------------------------------------------ */
void threadA(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    printk("Thread A started - waiting for button presses...\n");

    while (1)
    {
        /* Block here until the ISR gives the semaphore */
        k_sem_take(&button_sem, K_FOREVER);

        printk("Thread A woke up! Doing work now...\n");

        /* Your real work goes here */
        k_msleep(10); /* small debounce / processing time */
    }
}

/* ------------------------------------------------------------------ */
/* main() - configure button and start thread                         */
/* ------------------------------------------------------------------ */
void main(void)
{
    /* 1. Verify button device is ready */
    if (!device_is_ready(button.port))
    {
        printk("Error: Button device %s not ready\n", button.port->name);
        return;
    }

    /* 2. Configure pin as input */
    int ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0)
    {
        printk("Button config failed (%d)\n", ret);
        return;
    }

    /* 3. Register the direct ISR */
    GPIO_DIRECT_CALLBACK_ADD(button.port, button_isr);

    /* 4. Enable interrupt on falling edge (button on nucleo_wb55rg is active-low) */
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);
    if (ret < 0)
    {
        printk("Interrupt config failed (%d)\n", ret);
        return;
    }

    /* 5. Dummy callback structure required by the GPIO driver internals */
    static struct gpio_callback dummy_cb;
    gpio_init_callback(&dummy_cb, NULL, BIT(button.pin));
    gpio_add_callback(button.port, &dummy_cb);

    printk("Button direct ISR configured - press the user button!\n");

    /* 6. Start thread A */
    k_thread_create(&threadA_data, threadA_stack_area, STACKSIZE,
                    threadA, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);
}