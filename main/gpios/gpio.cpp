
#include "gpios/gpio.h"
#include "debug.h"


// Constructor: Initialize the GPIO pin and create a mux
GpioBase::GpioBase(gpio_num_t pin) : gpioPin(pin) 
{
    mutex = xSemaphoreCreateBinary();  // Create mux
    xSemaphoreGive(mutex);
}

// Destructor: Delete the mux
GpioBase::~GpioBase() {
    if (mutex != NULL) {
        vSemaphoreDelete(mutex);
    }
}

gpio_num_t GpioBase::getNumber() const
{
    return gpioPin;
}

void GpioBase::debugLogMutexAddress() const // remove
{
    bool isTaken = false;
    if (xSemaphoreTake(mutex, 0) == pdTRUE) {
        // Mutex was not taken, now you have taken it
        // Don't forget to release it after checking
        xSemaphoreGive(mutex);
        ESP_LOGI(GPIO_TAG, "Mutex is available\n");
        isTaken = false;
    } else {
        // Mutex is currently taken
        ESP_LOGI(GPIO_TAG, "Mutex is already taken\n");
        isTaken = true;
    }
    ESP_LOGE(GPIO_TAG, "Mutex pin: %d, address: %p, %s\n", (int)gpioPin, (void*)mutex, isTaken ? "is taken" : "not taken");
}