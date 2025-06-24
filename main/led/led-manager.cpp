
#include <algorithm>
#include <sstream>
#include "led/led-manager.h"
#include "debug.h"

LedManager::LedManager(GpioManager* gpioManager):
    gpios(gpioManager)
{  
}

void LedManager::init()
{
    rgbGreenLed = Led((GpioOutput*)gpios->get(LED_RGB_G), ALWAYS_OFF); 
    rgbRedLed = Led((GpioOutput*)gpios->get(LED_RGB_R), ALWAYS_OFF); 
    rgbBlueLed = Led((GpioOutput*)gpios->get(LED_RGB_B), ALWAYS_OFF); 
    whiteLed = Led((GpioOutput*)gpios->get(LED_W), ALWAYS_OFF);
    V(greenLed = Led((GpioOutput*)gpios->get(LED_G), ALWAYS_OFF));
    leds.push_back(&rgbRedLed);
    leds.push_back(&rgbGreenLed);
    leds.push_back(&rgbBlueLed);
    leds.push_back(&whiteLed);
    V(leds.push_back(&greenLed));
    //
    for (Led* led : leds) {
        led->initCounter();
        setProfile(led, ALWAYS_OFF);
    }
}

bool LedManager::updateLedTimersAndSetByMode(int rtosMinTime)
{
    #define HANDLE_LED(led, colorName) \
        do { \
            ESP_LOGV(LED_TAG, colorName " led time: %d - profile: %d - deadline: %d", \
                (int)(led).getPassedTimeMilliseconds(), \
                (int)(led).getProfile(), \
                (int)(led).getDeadlineMilliseconds()); \
            if ((led).updateTimer(rtosMinTime)) { \
                (led).setByProfile(); \
                ret = true; \
            } \
        } while(0)

    bool ret = false;
    HANDLE_LED(rgbRedLed,    "red rgb");
    HANDLE_LED(rgbGreenLed,  "green rgb");
    HANDLE_LED(rgbBlueLed,   "blue rgb");
    HANDLE_LED(whiteLed,     "white");
    V(HANDLE_LED(greenLed,   "green"));
    return ret;
}

bool LedManager::updateTimer(int rtosMinTime)
{
    return updateLedTimersAndSetByMode(rtosMinTime);
}

int32_t LedManager::getRealTimeToDeadlineMilliseconds() const 
{
    std::vector<int> deadlines;
    for (Led* led : leds) {
        deadlines.push_back(led->getRealTimeToDeadlineMilliseconds());
    }
    return *std::min_element(deadlines.begin(), deadlines.end());   
}

std::string LedManager::getLogCounter(std::string id) const
{
    std::vector<int> deadlines;
    for (Led* led : leds) {
        deadlines.push_back(led->getRealTimeToDeadlineMilliseconds());
    }
    int minDeadline = *std::min_element(deadlines.begin(), deadlines.end());

    std::stringstream ss;
    ss << id << ": LED deadlines [";
    for (size_t i = 0; i < deadlines.size(); ++i) {
        ss << deadlines[i];
        if (i < deadlines.size() - 1) ss << ", ";
    }
    ss << "], min: " << minDeadline << "\n";

    return ss.str();
}

void LedManager::initCounter()
{
    for (Led* led : leds) {
        led->initCounter();
    }
}

void LedManager::setProfile(Led* led, LED_PROFILE profile)
{
    bool referenceBlinkingStatus = false;
    if (profile == LED_PROFILE::BLINK_SLOW){
        for (Led* l : leds) {
            if (led != l && l->isBlinking()){
                l->copyTimestampsTo(led);
                referenceBlinkingStatus = l->isOn();
            }
        }
    }
    led->setProfile(Led::ProfileSetterKey{}, profile, referenceBlinkingStatus);
}
