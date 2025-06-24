#pragma once

#include "keyboard/i-keyboard-propagator.h"
#include "keyboard/i-keyboard-listener.h"
#include <set>


class KeyboardPropagator: public IKeyboardPropagator
{
public:
    KeyboardPropagator() = default;
    ~KeyboardPropagator() = default;
    void attachKeyboardListener(IKeyboardListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) override;
    void detachKeyboardListener(IKeyboardListener *listener) override;
    void fireScrollButtonPressure() override;
    void fireOnOffButtonPressure() override;
    void fireScrollButtonQuickRelease() override;
    void fireScrollButtonLongRelease() override;
    void fireOnOffButtonQuickRelease() override;
    void fireOnOffButtonLongRelease() override;
    void fireServiceAccessSequence() override;

    void howManyKeyboardListeners();

private:
    std::multiset<std::pair<PRIORITY, IKeyboardListener*>, std::greater<>> ms;

};


