#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <iostream>
#include <freertos/queue.h>
#include "util/thread-events/handle-setter.h"
#include "debug.h"

class Sender: public HandleSetter {
public:
    Sender() = default;
    Sender(QueueHandle_t queue);    
    void sendFromISR(int value) const;
};



