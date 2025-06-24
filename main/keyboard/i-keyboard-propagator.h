#pragma once

#include <iostream>
#include "keyboard/i-keyboard-listener.h"
#include "util/listener-priority.h"

class IKeyboardPropagator 
{
public:
    IKeyboardPropagator() = default;
    virtual ~IKeyboardPropagator() = default;
    virtual void attachKeyboardListener(IKeyboardListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) = 0;
    virtual void detachKeyboardListener(IKeyboardListener *listener) = 0;
    virtual void fireScrollButtonPressure() = 0;
    virtual void fireOnOffButtonPressure() = 0;
    virtual void fireScrollButtonQuickRelease() = 0;
    virtual void fireScrollButtonLongRelease() = 0;
    virtual void fireOnOffButtonQuickRelease() = 0;
    virtual void fireOnOffButtonLongRelease() = 0;
    virtual void fireServiceAccessSequence() = 0;
};

