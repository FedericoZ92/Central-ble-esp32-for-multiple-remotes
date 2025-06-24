#pragma once

#include "keyboard/i-keyboard-listener.h"
#include "keyboard/keyboard-propagator.h"

class KeyboardListener : public IKeyboardListener {
public:
    KeyboardListener() = default;
    ~KeyboardListener() = default;
    KeyboardListener(IKeyboardPropagator* propagator);


    void addPropagator(IKeyboardPropagator* propagator);

    void removeMeFromTheList();

private:
    IKeyboardPropagator* propagator_;
    static int static_number_;
    int number_;
};
