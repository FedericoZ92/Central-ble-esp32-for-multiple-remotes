#pragma once


#include "util/task.h"
#include "keyboard/button/button.h"


#define BUTTON_EVENTS_QUEUE_SIZE 10


class ButtonTask : public SharedTask {
public:
    ButtonTask(std::string name, void* pvParameters);
    void gpioRisingEdge();
    void gpioFallingEdge();

    int64_t debugDuration = 0;

private:
    void run() override;
    
    Button* button;
    int64_t startTime = 0; 
    int64_t currentTime = 0; 
    int64_t duration = 0;    
    QueueHandle_t buttonEventQueue;
};

