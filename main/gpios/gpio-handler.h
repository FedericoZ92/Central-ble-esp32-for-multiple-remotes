#pragma once

#include <driver/gpio.h>
#include "esp_attr.h"
#include "gpios/gpios.h"

class GpioHandler {
public:
    GpioHandler(gpio_num_t pin, void* params);
    void init();  // Initialize the GPIO and interrupt

    // The callback method that will be called when the GPIO event occurs
    static void IRAM_ATTR gpio_isr_handler(void* arg);

private:
    volatile int lastLevel_ = -1;
    gpio_num_t pin_;
    void* params_;
};

