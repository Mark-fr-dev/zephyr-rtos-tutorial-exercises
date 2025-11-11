#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define SLEEP_TIME_MS 1000

/* Devicetree node identifier for "led0" alias*/
#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE DT_ALIAS(sw0)

#if (!DT_NODE_HAS_STATUS(LED0_NODE, okay))
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if (!DT_NODE_HAS_STATUS(SW0_NODE, okay))
#error "Unsupported board: sw0 devicetree alias is not defined" 
#endif

/*Extract GPIO information */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);




void main(void)
{
   
    int ret; /* Variable to store return values of gpio functions (0 = success) */

    /* Check if the GPIO controller device (e.g. &gpiob) is ready after binding.
       device_is_ready() returns false if the device failed to probe or is disabled. */
    if (!device_is_ready(led.port)) {
        printk("Error: LED device %s is not ready\n", led.port->name);
        return; /* Abort main – the kernel will keep running other threads */
    }
    if (!device_is_ready(button.port))
    {
        printk("Error: Button device %s is not ready\n", button.port->name);
        return; /* Abort main – the kernel will keep running other threads */
    }

    /* Configure the pin as output and set initial state according to the
       device-tree flag (GPIO_ACTIVE_HIGH in your case → high = LED on).
       GPIO_OUTPUT_ACTIVE means "drive high on startup" (LED turns on immediately). */
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    ret |= gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_UP);

    if (ret < 0)
    {
        printk("GPIO config failed\n");
        return;
    }

    bool led_is_on = true;

    while (1) {
        if (gpio_pin_get_dt(&button) == 0) { // Button pressed (active high)
            if (led_is_on) {
                gpio_pin_set_dt(&led, 0); // Turn off LED
                led_is_on = false;
            } else {
                gpio_pin_set_dt(&led, 1); // Turn on LED
                led_is_on = true;
            }
            // Debounce delay
            k_msleep(200);
            // Wait for button release
            while (gpio_pin_get_dt(&button) == 0) {
                k_msleep(10);
            }
        }
        
       // k_msleep(SLEEP_TIME_MS);
    }
}