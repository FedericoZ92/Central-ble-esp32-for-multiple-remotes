
#include "keyboard/keyboard-propagator.h"
#include <functional>
#include "debug.h"


void KeyboardPropagator::attachKeyboardListener(IKeyboardListener *listener, PRIORITY priority)
{
    ms.emplace(priority, listener);
}

void KeyboardPropagator::detachKeyboardListener(IKeyboardListener *listener) 
{
    auto it = std::find_if(ms.begin(), ms.end(), [listener](const std::pair<PRIORITY, IKeyboardListener*>& pair) {
        return pair.second == listener;
    });

    if (it != ms.end()) {
        ms.erase(it); // Remove the item
    }
}

void KeyboardPropagator::fireScrollButtonPressure()
{
    ESP_LOGI(BUTTON_TAG, "fire scroll button pressure!");
    for (const auto& pair : ms) {
        pair.second->updateScrollButtonPressure();
    }
}

void KeyboardPropagator::fireOnOffButtonPressure()
{
    ESP_LOGI(BUTTON_TAG, "fire on off button pressure!");
    for (const auto& pair : ms) {
        pair.second->updateOnOffButtonPressure();
    }
}

void KeyboardPropagator::fireScrollButtonQuickRelease()
{
    ESP_LOGI(BUTTON_TAG, "fire scroll button quick release!");
    for (const auto& pair : ms) {
        pair.second->updateScrollButtonQuickRelease();
    }
}

void KeyboardPropagator::fireScrollButtonLongRelease()
{
    ESP_LOGI(BUTTON_TAG, "fire scroll button long release!");
    for (const auto& pair : ms) {
        pair.second->updateScrollButtonLongRelease();
    }
}

void KeyboardPropagator::fireOnOffButtonQuickRelease()
{
    ESP_LOGI(BUTTON_TAG, "fire on off button quick release!");
    for (const auto& pair : ms) {
        pair.second->updateOnOffButtonQuickRelease();
    }
}

void KeyboardPropagator::fireOnOffButtonLongRelease()
{
    ESP_LOGI(BUTTON_TAG, "fire on off button long release!");
    for (const auto& pair : ms) {
        pair.second->updateOnOffButtonLongRelease();
    }
}

void KeyboardPropagator::fireServiceAccessSequence()
{
    ESP_LOGI(BUTTON_TAG, "fire service access sequence!");
    for (const auto& pair : ms) {
        pair.second->updateServiceAccessSequence();
    }
}

void KeyboardPropagator::howManyKeyboardListeners() 
{
    ESP_LOGI(MAIN_TAG, "There are %d keyboard listeners", (int)ms.size());
}