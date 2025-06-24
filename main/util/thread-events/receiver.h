#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <iostream>
#include <freertos/queue.h>
#include "util/thread-events/handle-setter.h"
#include "debug.h"

class Receiver: public HandleSetter {
public:
    Receiver() = default;
    Receiver(QueueHandle_t queue);
    int receive() const;
    bool receiveWithTimeout(uint16_t timeoutMs) const;
};


