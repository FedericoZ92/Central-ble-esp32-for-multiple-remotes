
#include "debug.h"
//i2c
#include "gpios/gpio-handler.h"
//gpio
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/rtc_io.h>
//button
#include "keyboard/button/button-task.h"


// Constructor to initialize the GPIO pin
GpioHandler::GpioHandler(gpio_num_t pin, void* params) : pin_(pin), params_(params) {}

// Initialize the GPIO and set up the interrupt service
void GpioHandler::init() 
{
    // Install the GPIO ISR service, if not already installed
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    // Attach the static ISR handler for the pin
    gpio_isr_handler_add(pin_, gpio_isr_handler, this);
}

// The static ISR handler method
void IRAM_ATTR GpioHandler::gpio_isr_handler(void* arg) 
{
    GpioHandler* handler = static_cast<GpioHandler*>(arg);
    // Read the GPIO level
    int level = gpio_get_level( handler->pin_);
    ButtonTask* buttonTask = (ButtonTask*) handler->params_;

    if (level == 1) {
        // Rising edge detected
        buttonTask->gpioRisingEdge();
    } else {
        // Falling edge detected
        buttonTask->gpioFallingEdge();
    }
}
