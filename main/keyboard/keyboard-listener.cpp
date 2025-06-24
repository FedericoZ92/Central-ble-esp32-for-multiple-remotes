
#include "debug.h"
#include "keyboard/i-keyboard-propagator.h"
#include "keyboard/keyboard-listener.h"

KeyboardListener::KeyboardListener(IKeyboardPropagator* propagator): propagator_(propagator)
{
    this->propagator_->attachKeyboardListener(this);
    this->number_ = KeyboardListener::static_number_;
}

void KeyboardListener::addPropagator(IKeyboardPropagator* propagator)
{
    ESP_LOGI(MAIN_TAG, "add keyboard propagator");
    this->propagator_ = propagator;
    this->propagator_->attachKeyboardListener(this);                
}

void KeyboardListener::removeMeFromTheList() 
{
    propagator_->detachKeyboardListener(this);
}

int KeyboardListener::static_number_ = 0;