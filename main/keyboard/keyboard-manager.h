#pragma once

#include "keyboard/button/button.h"
#include "keyboard/button/button-task.h"
#include "keyboard/button/button-listener.h"
#include "keyboard/service-access/controller.h"
#include "keyboard/keyboard-propagator.h"


class KeyboardManager: public ButtonListener, public KeyboardPropagator
{
public:
    KeyboardManager(Button* scroll, ButtonTask* scrollTask, Button* onOff, ButtonTask* onOffTask);   
    void init(); 

    //button listener
    void updateButtonPressed(ButtonType type) final;
    void updateButtonQuickReleased(ButtonType type) final;
    void updateButtonLongReleased(ButtonType type) final;
    void updateButtonLongReleasedAfterTimeout(ButtonType type) final;

private:
    Button* onOffButton; 
    ButtonTask* onOffButtonMonitoringTask;
    Button* scrollButton;    
    ButtonTask* scrollButtonMonitoringTask;
    ServiceAccessController service;
};
