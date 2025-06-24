#include "util/thread-events/sender.h"

Sender::Sender(QueueHandle_t queue):
    HandleSetter(queue)
{
}

void Sender::sendFromISR(int value) const 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xQueueSendFromISR(queueHandle, &value, &xHigherPriorityTaskWoken) == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        ESP_LOGE("Sender", "Failed to send to queue from ISR");
    }
}


