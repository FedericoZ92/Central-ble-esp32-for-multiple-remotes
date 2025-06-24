#pragma once

#include "bluetooth/i-ble-listener.h"
#include "bluetooth/ble-propagator.h"

class BleListener : public IBleListener {
public:
    BleListener() = default;
    BleListener(IBlePropagator* propagator);
    virtual ~BleListener() = default;

    void addPropagator(IBlePropagator* propagator);

    void removeMeFromTheList();

private:
    IBlePropagator* propagator_;
    static int static_number_;
    int number_;
};
