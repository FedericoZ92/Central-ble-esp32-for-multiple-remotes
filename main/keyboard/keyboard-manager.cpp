
#include "keyboard/keyboard-manager.h"
#include "debug.h"


KeyboardManager::KeyboardManager(Button* scroll, ButtonTask* scrollTask, Button* onOff, ButtonTask* onOffTask):
    onOffButton(onOff),
    onOffButtonMonitoringTask(onOffTask),
    scrollButton(scroll),
    scrollButtonMonitoringTask(scrollTask)
{
    onOffButton->attachButtonListener(this, PRIORITY_VERY_HIGH);
    if(scrollButton){
        scrollButton->attachButtonListener(this, PRIORITY_VERY_HIGH);
    }
}

void KeyboardManager::init()
{
    onOffButtonMonitoringTask->init();
    if(scrollButton){
        scrollButtonMonitoringTask->init();
    }
}

void KeyboardManager::updateButtonPressed(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "keyboard, update button pressure");
    if(service.checkServiceAccessSequencePostNotifyPressure(type)){
        // do nothing: the user is inserting the service sequence, hence regular key pressures as standalone events are ignored 
    }else if (type == ButtonType::SCROLL){
        fireScrollButtonPressure();
    }else if (type == ButtonType::HARDWARE){
        //for now, nobody cares
    }
}

void KeyboardManager::updateButtonQuickReleased(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "keyboard, update button quick release");
    if (service.checkServiceAccessSequencePostNotifyRelease(type)){
        // if returns true the user has inserted the service sequence on a quick release of scroll button event,
        // if so we trigger the sequence event, though it's highly unlikely (considering deboucing too... must have done it super fast somehow)
        fireServiceAccessSequence();
    }else if (type == ButtonType::SCROLL){
        if (!service.isSequenceInitiatedByHwButton()){
            fireScrollButtonQuickRelease();
        }
    }else if (type == ButtonType::HARDWARE){
        if (!service.isSequenceInitiatedByScrollButton()){
            fireOnOffButtonQuickRelease(); //the 'back' on ui
        }
    }
}

void KeyboardManager::updateButtonLongReleased(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "keyboard, update button long release");
    if (type == ButtonType::SCROLL){
        if (!service.isSequenceInitiatedByHwButton()){
            fireScrollButtonLongRelease();
        }
    }else if (type == ButtonType::HARDWARE){
        // in this case, the button tells the gpio manager directly to shut down
        fireOnOffButtonLongRelease();
        
    }
}

void KeyboardManager::updateButtonLongReleasedAfterTimeout(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "keyboard, update button long release after timeout");
    if (service.checkServiceAccessSequencePostNotifyRelease(type)){
        // if true the service sequence has been detected
        fireServiceAccessSequence();
    }
}
