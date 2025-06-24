
#include "keyboard/button/button.h"
#include "debug.h"


Button::Button(gpio_num_t gpioNumber, int longPressureMs, int shortPressureMs, int defaultLevel, ButtonType type):
    gpioNumber_(gpioNumber), longPressureMs_(longPressureMs), shortPressureMs_(shortPressureMs), type_(type)
{}

int Button::getGpioLevel() const
{
    return gpio_get_level(gpioNumber_);
}

int Button::getLongPressureMs() const
{
    return longPressureMs_;
}

int Button::getShortPressureMs() const
{
    return shortPressureMs_;
}

gpio_num_t Button::getGpioNumber() const
{
    return gpioNumber_;
}

ButtonType Button::getType() const
{
    return type_;
}

ButtonStatus_t Button::getStatus() const
{
    return status;
}

void Button::setStatus(ButtonStatus_t value)
{
    status = value;
}