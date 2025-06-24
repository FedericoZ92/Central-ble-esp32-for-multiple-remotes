#pragma once

#include "gpios/gpio.h"

class GpioOutput : public GpioBase 
{
public:
    GpioOutput(gpio_num_t pin);      
    ~GpioOutput() = default;                   

    //void configureGpio() override;   // Override to configure as output

    // Set the GPIO output level (with mutex protection)
    void setOutputLevel(int level);

    // Get the current output level (with mutex protection)
    int getOutputLevel();

private:
    int outputLevel;  // To track the output level
};