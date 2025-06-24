#pragma once

#include "gpios/gpios.h"
#include "keyboard/button/i-button-propagator.h"
#include "keyboard/button/i-button-listener.h"
#include <set>


class ButtonPropagator: public IButtonPropagator
{
public:
    ButtonPropagator() = default;
    ~ButtonPropagator() = default;
    void attachButtonListener(IButtonListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) override;
    void detachButtonListener(IButtonListener *listener) override;
    void fireButtonPressed(ButtonType type) override;
    void fireButtonQuickReleased(ButtonType type) override;
    void fireButtonLongReleased(ButtonType type) override;
    void fireButtonLongReleasedAfterTimeout(ButtonType type) override;

    void howManyButtonListeners();

private:
    std::multiset<std::pair<PRIORITY, IButtonListener*>, std::greater<>> ms;

};


