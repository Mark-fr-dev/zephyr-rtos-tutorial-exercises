#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define SLEEP_TIME_MS 1000

/* Devicetree node identifier for "led0" alias*/
#define LED0_NODE DT_ALIAS(led1)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0 ""
#define PIN 0
#define FLAGS 0
#endif

void main(void)
{
    /* Declare a gpio_dt_spec structure that holds all device-tree information
       for the LED (GPIO controller, pin number, active-high/low, etc.).
       GPIO_DT_SPEC_GET() automatically extracts the data from the node
       pointed to by LED0_NODE (which is DT_ALIAS(led0) → blue LED). */
    struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
    int ret; /* Variable to store return values of gpio functions (0 = success) */

    /* Check if the GPIO controller device (e.g. &gpiob) is ready after binding.
       device_is_ready() returns false if the device failed to probe or is disabled. */
    if (!device_is_ready(led.port)) {
        printk("Error: LED device %s is not ready\n", led.port->name);
        return; /* Abort main – the kernel will keep running other threads */
    }

    /* Configure the pin as output and set initial state according to the
       device-tree flag (GPIO_ACTIVE_HIGH in your case → high = LED on).
       GPIO_OUTPUT_ACTIVE means "drive high on startup" (LED turns on immediately). */
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure %s pin %d\n",
               ret, led.port->name, led.pin);
        return;
    }

    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(SLEEP_TIME_MS);
    }
}