
#include "util/task.h"

#include "debug.h"

// Constructor implementation
SharedTask::SharedTask(std::string name, uint32_t size, UBaseType_t priority, void* pvParameters):
    parameters(pvParameters),
    taskName(name),
    stackSize(size),
    taskPriority(priority)
{}

// Create task method
void SharedTask::init() 
{
    BaseType_t outcome; 
    // Create the task and pass 'this' as the parameter
    outcome = xTaskCreate(taskFunctionWrapper, taskName.c_str(), stackSize, this, taskPriority, NULL);
    
    if (outcome != pdPASS){ //this è il parametro
        // Task creation failed
        ESP_LOGE(MAIN_TAG, "xTaskCreate failed for task: %s, error: %d", taskName.c_str(), (int) outcome);     
    }else{
        ESP_LOGI(MAIN_TAG, "xTaskCreate successful for task: %s", taskName.c_str());
    }
}

// Wrapper function for task creation
void SharedTask::taskFunctionWrapper(void* pvParameters)
{
    // Unwrap the object and call the run method
    SharedTask* task = static_cast<SharedTask*>(pvParameters);
    ESP_LOGI(MAIN_TAG, "task->run()\n"); 
    task->run();
}