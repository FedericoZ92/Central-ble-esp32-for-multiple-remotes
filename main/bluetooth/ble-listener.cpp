
#include "debug.h"
#include "bluetooth/i-ble-propagator.h"
#include "bluetooth/ble-listener.h"


BleListener::BleListener(IBlePropagator* propagator): propagator_(propagator)
{
    this->propagator_->attachBleListener(this);
    this->number_ = BleListener::static_number_;
}

void BleListener::addPropagator(IBlePropagator* propagator)
{
    ESP_LOGI(MAIN_TAG, "add ble propagator");
    this->propagator_ = propagator;
    this->propagator_->attachBleListener(this);                
}

void BleListener::removeMeFromTheList() 
{
    propagator_->detachBleListener(this);
}

int BleListener::static_number_ = 0;