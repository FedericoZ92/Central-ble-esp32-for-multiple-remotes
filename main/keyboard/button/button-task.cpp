
#include "keyboard/button/button-task.h"
#include "debug.h"
#include "keyboard/button/button.h"
#include <esp_timer.h>
#include <driver/gpio.h>
#include <freertos/queue.h>

#define DEBOUNCE_DELAY_MS 100

// Constructor
ButtonTask::ButtonTask(std::string name, void* pvParameters) :
    SharedTask(name, configMINIMAL_STACK_SIZE + 4096, 5, nullptr),
    button((Button*) pvParameters)
{
    buttonEventQueue = xQueueCreate(BUTTON_EVENTS_QUEUE_SIZE, sizeof(int));
}


void ButtonTask::run() 
{
    ESP_LOGV(BUTTON_TAG, "Initiating button task, type: %d", button->getType());

    gpio_int_type_t lastEvent = GPIO_INTR_MAX;  // Store last event type
    TickType_t latestEventTime = 0;               // Store last event time
    gpio_int_type_t event;

    while (1) {
        switch (button->getStatus()){
        case LOOSE:
            if (xQueueReceive(buttonEventQueue, &event, portMAX_DELAY)) {
                // Ignore duplicate events within debounce period
                TickType_t now = xTaskGetTickCount();  // Get current time in ticks
                if (event == lastEvent && (now - latestEventTime) < pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)) {
                    ESP_LOGD(BUTTON_TAG, "debounce!");
                    continue;
                }
                lastEvent = event;
                latestEventTime = now;
                //debounce check end
                switch (event) {
                    case GPIO_INTR_POSEDGE: // Rising edge (button released)
                        ESP_LOGW(BUTTON_TAG, "fire released while released, only matters for secret seqeunce correct detection");
                        button->fireButtonLongReleasedAfterTimeout(button->getType());
                        break;
                    case GPIO_INTR_NEGEDGE: // Falling edge (button pressed)
                        button->setStatus(PRESSED);
                        //
                        startTime = esp_timer_get_time();
                        ESP_LOGW(BUTTON_TAG, "fire b. pressed");
                        button->fireButtonPressed(button->getType());
                        ESP_LOGV(BUTTON_TAG, "Button falling time now: %lli", (long long) startTime);
                        break;
                    default:
                        ESP_LOGV(BUTTON_TAG, "Event not handled");
                }
            }
            break;
        case PRESSED:
            if (xQueueReceive(buttonEventQueue, &event, pdMS_TO_TICKS(10))) {
                //ESP_LOGD(BUTTON_TAG, "pr receive");
                // Ignore duplicate events within debounce period
                TickType_t now = xTaskGetTickCount();  // Get current time in ticks
                if (event == lastEvent && (now - latestEventTime) < pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)) {
                    ESP_LOGD(BUTTON_TAG, "debounce!");
                    continue;
                }
                lastEvent = event;
                latestEventTime = now;
                //debounce check end
                switch (event) {
                    case GPIO_INTR_POSEDGE: // Rising edge (button released)
                        button->setStatus(LOOSE);
                        ESP_LOGD(BUTTON_TAG, "go LOOSE");
                        //
                        currentTime = esp_timer_get_time();
                        ESP_LOGD(BUTTON_TAG, "Button rising time now: %lli", (long long) currentTime);
                        duration = (currentTime - startTime)/1000; //us
                        if (SHORT_BUTTON_PERIOD < duration && duration < button->getLongPressureMs()){
                            ESP_LOGW(BUTTON_TAG, "fire quick re.");
                            button->fireButtonQuickReleased(button->getType());
                            startTime = currentTime;
                            duration = 0;
                        }
                        break;
                    case GPIO_INTR_NEGEDGE: // Falling edge (button pressed)
                        ESP_LOGD(BUTTON_TAG, "pressure while pressed, do nothing, cannot have happened");
                        break;
                    default:
                        ESP_LOGV(BUTTON_TAG, "Event not handled");
                }
            }else {
                //ESP_LOGD(BUTTON_TAG, "pr not");
                currentTime = esp_timer_get_time();
                duration = (currentTime - startTime)/1000; //us
                if (duration > button->getLongPressureMs()){
                    ESP_LOGW(BUTTON_TAG, "fire long released time now: %lli", (long long) currentTime);
                    button->fireButtonLongReleased(button->getType());
                    startTime = currentTime;
                    duration = 0;
                    button->setStatus(LOOSE);
                    ESP_LOGD(BUTTON_TAG, "go LOOSE");                    
                }
            }  
            break; 
        default:
            break;
        }
    } 
    vTaskDelete(NULL);    
}

void ButtonTask::gpioRisingEdge()
{
    gpio_int_type_t event = gpio_int_type_t::GPIO_INTR_POSEDGE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(buttonEventQueue, &event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Request context switch if necessary
}

void ButtonTask::gpioFallingEdge()
{
    gpio_int_type_t event = gpio_int_type_t::GPIO_INTR_NEGEDGE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(buttonEventQueue, &event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Request context switch if necessary
}

