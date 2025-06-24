#pragma once


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <iostream>

class SharedTask {
public:
    SharedTask(std::string name, uint32_t size, UBaseType_t priority, void* pvParameters);
    
    void init();

protected:
    virtual void run() = 0;
    void* parameters;

private:
    static void taskFunctionWrapper(void* pvParameters);

    std::string taskName = "";
    uint32_t stackSize = 0;
    UBaseType_t taskPriority = 0;

};

