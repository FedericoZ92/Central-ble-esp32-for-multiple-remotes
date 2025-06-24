#pragma once

#include "keyboard/button/button-propagator.h"
#include <driver/gpio.h>
#include "keyboard/button/button-defs.h"

#define LOW_LEVEL 0
#define HIGH_LEVEL 1

#define LONG_SCROLL_BUTTON_PERIOD 1000
#define LONG_ONOFF_BUTTON_PERIOD 3000
#define SHORT_BUTTON_PERIOD 50

typedef enum{
    LOOSE,
    PRESSED
}ButtonStatus_t;

class Button: public ButtonPropagator
{
public:
    Button(gpio_num_t gpioNumber, int longPressureMs, int shortPressureMs, int defaultLevel, ButtonType type);
    int getGpioLevel() const;
    int getLongPressureMs() const;
    int getShortPressureMs() const;
    gpio_num_t getGpioNumber() const;
    ButtonType getType() const;

    ButtonStatus_t getStatus() const;
    void setStatus(ButtonStatus_t value);

private:
    gpio_num_t gpioNumber_;
    int longPressureMs_;
    int shortPressureMs_;
    ButtonType type_;
    ButtonStatus_t status = LOOSE;
};