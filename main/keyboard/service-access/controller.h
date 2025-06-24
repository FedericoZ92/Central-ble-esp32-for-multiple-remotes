#pragma once

#include <cstdio>
#include "keyboard/button/button-defs.h"

//the service sequence is: press scroll, press hw 3 times (quick press), release scroll
class ServiceAccessController{
public:
    ServiceAccessController() = default;
    ~ServiceAccessController() = default; 

    // all subject to change if sequence change
    bool checkServiceAccessSequencePostNotifyPressure(ButtonType type);
    bool checkServiceAccessSequencePostNotifyRelease(ButtonType type);
    void resetSequence();

    bool isSequenceInitiatedByHwButton() const; 
    bool isSequenceInitiatedByScrollButton() const;

private:
    uint8_t step = 0;
};