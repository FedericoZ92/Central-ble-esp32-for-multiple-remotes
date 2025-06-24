#include "util/thread-events/receiver.h"

Receiver::Receiver(QueueHandle_t queue):
    HandleSetter(queue)
{
}

int Receiver::receive() const 
{
    int value;
    if (xQueueReceive(queueHandle, &value, portMAX_DELAY) == pdTRUE) {
        ESP_LOGI("Receiver", "Received: %d", value);
    } else {
        ESP_LOGE("Receiver", "Failed to receive from queue");
    }
    return value;
}

bool Receiver::receiveWithTimeout(uint16_t timeoutMs) const
{
    int value;
    return (bool) (xQueueReceive(queueHandle, &value, timeoutMs) == pdTRUE);
}
