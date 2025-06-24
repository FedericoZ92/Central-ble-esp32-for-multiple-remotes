
#include "debug.h"
#include "keyboard/button/i-button-propagator.h"
#include "keyboard/button/button-listener.h"


ButtonListener::ButtonListener(IButtonPropagator* propagator): propagator_(propagator)
{
    this->propagator_->attachButtonListener(this);
    this->number_ = ButtonListener::static_number_;
}

void ButtonListener::addPropagator(IButtonPropagator* propagator)
{
    ESP_LOGI(MAIN_TAG, "add button propagator");
    this->propagator_ = propagator;
    this->propagator_->attachButtonListener(this);                
}

void ButtonListener::removeMeFromTheList() 
{
    propagator_->detachButtonListener(this);
}

int ButtonListener::static_number_ = 0;