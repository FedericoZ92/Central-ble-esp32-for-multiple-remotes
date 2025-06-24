
#include "gpios/output-gpio.h"
#include "debug.h"


// Constructor: Calls the base class constructor and configures GPIO
GpioOutput::GpioOutput(gpio_num_t pin) : 
    GpioBase(pin), 
    outputLevel(0) {
}

// Set the GPIO output level (with mutex protection)
void GpioOutput::setOutputLevel(int level) 
{
    if (xSemaphoreTake(mutex, portMAX_DELAY)) {
        gpio_set_level(gpioPin, level);
        outputLevel = level;  // Track the output level
        ESP_LOGD(GPIO_TAG, "GPIO %d set to %d", gpioPin, level);
        xSemaphoreGive(mutex);
    } else {
        ESP_LOGE(GPIO_TAG, "Failed to take mutex");
    }
}

// Get the current output level (with mutex protection)
int GpioOutput::getOutputLevel() {
    int level = 0;
    if (xSemaphoreTake(mutex, portMAX_DELAY)) {
        level = outputLevel;
        xSemaphoreGive(mutex);
    } else {
        ESP_LOGE(GPIO_TAG, "Failed to take mutex");
    }
    return level;
}