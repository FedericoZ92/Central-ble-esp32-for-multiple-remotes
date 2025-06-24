#include "util/thread-events/handle-setter.h"

HandleSetter::HandleSetter(QueueHandle_t queue): 
    queueHandle(queue)
{        
}

void HandleSetter::setHandle(QueueHandle_t queue)
{
    queueHandle = queue;
}
