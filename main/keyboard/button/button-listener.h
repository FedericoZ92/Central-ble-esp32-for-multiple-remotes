#pragma once

#include "keyboard/button/i-button-listener.h"
#include "keyboard/button/button-propagator.h"

class ButtonListener : public IButtonListener {
public:
    ButtonListener() = default;
    ~ButtonListener() = default;
    ButtonListener(IButtonPropagator* propagator);

    void addPropagator(IButtonPropagator* propagator);

    void removeMeFromTheList();

private:
    IButtonPropagator* propagator_;
    static int static_number_;
    int number_;
};
