#pragma once

#include <iostream>

class IKeyboardListener
{
public:
    IKeyboardListener() = default;
    virtual ~IKeyboardListener() = default;
    virtual void updateScrollButtonPressure() = 0;
    virtual void updateOnOffButtonPressure() = 0;
    virtual void updateScrollButtonQuickRelease() = 0;
    virtual void updateScrollButtonLongRelease() = 0;
    virtual void updateOnOffButtonQuickRelease() = 0;
    virtual void updateOnOffButtonLongRelease() = 0;
    virtual void updateServiceAccessSequence() = 0;
};
