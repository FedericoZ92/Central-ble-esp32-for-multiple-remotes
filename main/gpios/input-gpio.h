#pragma once

#include "gpios/gpio.h"

class GpioInput : public GpioBase 
{
public:
    GpioInput(gpio_num_t pin);
    ~GpioInput() = default;                 

    //void configureGpio() override;  // Override to configure as input

    // Read the GPIO input level (with mutex protection)
    int readInputLevel();
};