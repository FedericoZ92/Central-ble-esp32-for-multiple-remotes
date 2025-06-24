
#include "gpios/input-gpio.h"
#include "debug.h"

// Constructor: Calls the base class constructor and configures GPIO
GpioInput::GpioInput(gpio_num_t pin) : GpioBase(pin) {
}

// Read the GPIO input level (with mux protection)
int GpioInput::readInputLevel() {
    int level = 0;
    if (xSemaphoreTake(mutex, portMAX_DELAY)) {
        level = gpio_get_level(gpioPin);
        ESP_LOGD(GPIO_TAG, "GPIO %d read level: %d", gpioPin, level);
        xSemaphoreGive(mutex);
    } else {
        ESP_LOGE(GPIO_TAG, "Failed to take mutex");
    }
    return level;
}