#pragma once

#include <iostream>
#include "keyboard/button/i-button-listener.h"
#include "keyboard/button/button-defs.h"
#include "util/listener-priority.h"

class IButtonPropagator {
    public:
    virtual void attachButtonListener(IButtonListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) = 0;
    virtual void detachButtonListener(IButtonListener *listener) = 0;
    virtual void fireButtonPressed(ButtonType type) = 0;
    virtual void fireButtonQuickReleased(ButtonType type) = 0;
    virtual void fireButtonLongReleased(ButtonType type) = 0;
    virtual void fireButtonLongReleasedAfterTimeout(ButtonType type) = 0;
};

