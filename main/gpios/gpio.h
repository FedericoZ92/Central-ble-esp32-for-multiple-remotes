#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

class GpioBase 
{
public:
    GpioBase(gpio_num_t pin);   
    ~GpioBase();        

    // Virtual function to configure GPIO (implemented in derived classes)
    // virtual void configureGpio() = 0;
    gpio_num_t getNumber() const;

    void debugLogMutexAddress() const;

protected:
    gpio_num_t gpioPin;         // GPIO pin number
    SemaphoreHandle_t mutex;    // Mutex for concurrency control
};