#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/string.h>

/********************** */
/*IO related definitions*/
/********************** */
#define SLEEP_TIME_MS 1000
/* Devicetree node identifier for "led0" alias*/
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

#if (!DT_NODE_HAS_STATUS(LED0_NODE, okay))
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if (!DT_NODE_HAS_STATUS(LED1_NODE, okay))
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

/*Extract GPIO information */
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/********************** */
/*Thread related definitions*/
/********************** */
/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* Running time for threads*/
#define SLEEPTIME 500
#define BLUE_PERIOD_MS 500
#define GREEN_PERIOD_MS 2000

K_THREAD_STACK_DEFINE(blue_thread_stack_area, STACKSIZE);
static struct k_thread blue_thread_data;

/* Define the stack area for thread B*/
K_THREAD_STACK_DEFINE(green_thread_stack_area, STACKSIZE);
static struct k_thread green_thread_data;

void blue_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    printk("Blue: started\n");

    while (1)
    {
        gpio_pin_toggle_dt(&blue_led);
        k_msleep(BLUE_PERIOD_MS); // now truly 500 ms visible
        printk("Blue: yielding\n");
        k_thread_resume(&green_thread_data);
        k_thread_suspend(&blue_thread_data); // clean handoff
    }
}

void green_thread(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1); ARG_UNUSED(dummy2); ARG_UNUSED(dummy3);

    printk("Green: started\n");

    while (1)
    {
        if(gpio_pin_get_dt(&green_led))
        {
            gpio_pin_set_dt(&green_led, 0); // Turn off LED
        }
        else
        {
            gpio_pin_set_dt(&green_led, 1); // Turn on LED
        }
        k_msleep(GREEN_PERIOD_MS);
        printk("Green: Resuming blue thread\n");    
        k_thread_resume(&blue_thread_data);
        k_thread_suspend(&green_thread_data);
    }
}

void main(void)
{
    int ret;

    /* ---------- GPIO setup (must be here!!) ---------- */
    if (!device_is_ready(blue_led.port) || !device_is_ready(green_led.port))
    {
        printk("Error: one or both LED devices not ready\n");
        return;
    }

    ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_INACTIVE);   // start OFF
    ret |= gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE); // start OFF
    if (ret < 0)
    {
        printk("Error configuring GPIO pins\n");
        return;
    }

    /* ---------- Thread creation ---------- */
    k_thread_create(&blue_thread_data, blue_thread_stack_area,
                    K_THREAD_STACK_SIZEOF(blue_thread_stack_area),
                    blue_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT); // suspended

    k_thread_create(&green_thread_data, green_thread_stack_area,
                    K_THREAD_STACK_SIZEOF(green_thread_stack_area),
                    green_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT); // starts immediately

    k_thread_name_set(&blue_thread_data, "blue_thread");
    k_thread_name_set(&green_thread_data, "green_thread");

    printk("main: Green thread started — ping-pong begins!\n");
        k_thread_start(&green_thread_data);
        k_thread_start(&blue_thread_data);

    while (1) k_msleep(1000);
}