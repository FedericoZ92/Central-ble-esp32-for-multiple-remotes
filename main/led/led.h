#pragma once


#include <driver/gpio.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include "gpios/output-gpio.h"
#include "scheduler/counter.h"


#define PROPAGATOR_EVENT_BIT  (1 << 1)  // Bit for user-generated events

enum LED_PROFILE{
    ALWAYS_OFF,
    ALWAYS_ON,
    BLINK_SLOW,
    //BLINK_FAST,
    //...
};

enum LED_STATUS{
    LED_OFF = 0,
    LED_ON = 1,
    LED_BLINKING = 2
};

struct BlinkingRule{
    int offTimeMs;
    int onTimeMs;
};

struct LedProfile{
    LED_PROFILE profile;
    LED_STATUS status;
    BlinkingRule* blinkingRule;
};

extern BlinkingRule SlowBlinking;
extern BlinkingRule FastBlinking;
extern std::vector<LedProfile> ledProfiles;

class LedManager;

class Led : public Counter
{
public:
    Led() = default;
    Led(GpioOutput* gpio, LED_PROFILE defaultProfile);
    ~Led() = default;

    class ProfileSetterKey {
        friend class LedManager;  // Only LedManager can construct this key
        ProfileSetterKey() = default;
    };
    
    class OverrideKey {
        friend class LedManager;  // screen manager override
        OverrideKey() = default;
    };
    
    LED_PROFILE getProfile() const;

    void setByProfile();
    bool isOn() const;
    bool isBlinking() const;
    void setProfile(const ProfileSetterKey& key, LED_PROFILE profile, bool refrenceBlinkingStatus = false); // to be called by the update methods (listener classes)
    void overrideStatus(const OverrideKey& key, bool overrideOn, bool status);

private:
    GpioOutput* gpio_;
    LED_PROFILE activeProfile = ALWAYS_OFF;
    bool logicalStatus = false;
    bool overrideMode = false;

    void turnOn(); 
    void turnOff(); 
    bool isGpioOn() const;

};


