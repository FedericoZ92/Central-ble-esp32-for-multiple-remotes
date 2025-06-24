#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

class HandleSetter {
public:
    HandleSetter() = default;
    HandleSetter(QueueHandle_t queue);
    void setHandle(QueueHandle_t queue);

protected:
    QueueHandle_t queueHandle;

};



