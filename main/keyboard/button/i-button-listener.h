#pragma once

#include <iostream>

#include "keyboard/button/button-defs.h"

class IButtonListener
{
public:
    IButtonListener() = default;
    virtual ~IButtonListener() = default;
    virtual void updateButtonPressed(ButtonType type) = 0;
    virtual void updateButtonQuickReleased(ButtonType type) = 0;
    virtual void updateButtonLongReleased(ButtonType type) = 0;
    virtual void updateButtonLongReleasedAfterTimeout(ButtonType type) = 0;
};
