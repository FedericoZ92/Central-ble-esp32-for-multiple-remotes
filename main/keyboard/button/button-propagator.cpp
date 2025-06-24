
#include "keyboard/button/button-propagator.h"
#include <functional>
#include "debug.h"


void ButtonPropagator::attachButtonListener(IButtonListener *listener, PRIORITY priority)
{
    ms.emplace(priority, listener);
}

void ButtonPropagator::detachButtonListener(IButtonListener *listener) 
{
    auto it = std::find_if(ms.begin(), ms.end(), [listener](const std::pair<PRIORITY, IButtonListener*>& pair) {
        return pair.second == listener;
    });

    if (it != ms.end()) {
        ms.erase(it); // Remove the item
    }
}

void ButtonPropagator::fireButtonPressed(ButtonType type) 
{
    howManyButtonListeners();
    ESP_LOGI(MAIN_TAG, "fire Button Pressed! type: %d", type);
    for (const auto& pair : ms) {
        pair.second->updateButtonPressed(type);
    }    
}

void ButtonPropagator::fireButtonQuickReleased(ButtonType type)
{
    howManyButtonListeners();
    ESP_LOGI(MAIN_TAG, "fire Button Quick Released! type: %d", type);
    for (const auto& pair : ms) {
        pair.second->updateButtonQuickReleased(type);
    }
}

void ButtonPropagator::fireButtonLongReleased(ButtonType type)
{
    ESP_LOGI(MAIN_TAG, "fire Button Long Released! type: %d", type);
    for (const auto& pair : ms) {
        pair.second->updateButtonLongReleased(type);
    }
}

void ButtonPropagator::fireButtonLongReleasedAfterTimeout(ButtonType type)
{
    ESP_LOGI(MAIN_TAG, "fire Button Long Released aft. timeout! type: %d", type);
    for (const auto& pair : ms) {
        pair.second->updateButtonLongReleasedAfterTimeout(type);
    }
}

void ButtonPropagator::howManyButtonListeners() 
{
    ESP_LOGV(MAIN_TAG, "There are %d button listeners", (int)ms.size());
}


