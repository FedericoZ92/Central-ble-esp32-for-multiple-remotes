
#include "keyboard/service-access/controller.h"
#include "debug.h"


bool ServiceAccessController::checkServiceAccessSequencePostNotifyPressure(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "check service access sequence post notify pressure step: %d", (int) step);
    switch (step){
    case 0:
        if (type == ButtonType::SCROLL){
            step++;
            ESP_LOGI(BUTTON_TAG, "press step++ %d", step);
            return false;
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:                         
    default:
        if (type == ButtonType::SCROLL){
            step = 0;        
        }
        break;
    }

    return false; //no chance
}

bool ServiceAccessController::checkServiceAccessSequencePostNotifyRelease(ButtonType type)
{
    ESP_LOGI(BUTTON_TAG, "check service access sequence post notify release step: %d", (int) step);
    switch (step){
    case 0:
        //
        break;
    case 1:
    case 2:
    case 3:
        if (type == ButtonType::HARDWARE){
            step++;
            ESP_LOGI(BUTTON_TAG, "rel. step++ %d", (int) step);
            return false;
        }
        break;
    case 4:
        if (type == ButtonType::SCROLL){
            step = 0;
            ESP_LOGI(BUTTON_TAG, "rel. step %d ret true", (int) step);
            return true;
        }
        break;                            
    default:
        break;
    }
    step = 0;
    return false;
}

void ServiceAccessController::resetSequence()
{
    step = 0;
}

bool ServiceAccessController::isSequenceInitiatedByHwButton() const
{
    ESP_LOGI(BUTTON_TAG, "is sequence initiated by hw button: %d", (int) (step > 1));
    return step > 1; //hw button has possibly initiated the sequence
}

bool ServiceAccessController::isSequenceInitiatedByScrollButton() const
{
    ESP_LOGI(BUTTON_TAG, "is sequence initiated by scroll button: %d", (int) (step > 0));
    return step > 0; //scroll button has possibly initiated the sequence
}