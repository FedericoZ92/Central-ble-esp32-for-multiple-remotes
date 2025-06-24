
#include "led/led.h"
#include "debug.h"

BlinkingRule SlowBlinking ={
    .offTimeMs = 5000,
    .onTimeMs = 1000,
};

BlinkingRule FastBlinking = {
    .offTimeMs = 5000,
    .onTimeMs = 500,
};

std::vector<LedProfile> ledProfiles = {
    {
        ALWAYS_OFF,
        LED_OFF,
        nullptr
    },
    {
        ALWAYS_ON,
        LED_ON,
        nullptr
    },
    {
        BLINK_SLOW,
        LED_BLINKING,
        &SlowBlinking
    },
    /*{
        BLINK_FAST,
        LED_BLINKING,
        &FastBlinking
    },*/
};

Led::Led(GpioOutput* gpio, LED_PROFILE defaultProfile):
    gpio_(gpio), activeProfile(defaultProfile)
{
    setDeadlineMillisecs(DEADLINE_INF);
    setByProfile();
};

void Led::setProfile(const ProfileSetterKey& key, LED_PROFILE profile, bool refrenceBlinkingStatus) 
{
    (void)key;
    bool isDifferent = (activeProfile != profile);
    activeProfile = profile;
    ESP_LOGD(LED_TAG, "set profile: %d, it is %s", profile, isDifferent ? "different" : "not different" );
    if (isDifferent ){ // made so it synchs to an already blinking led
        if(profile != LED_PROFILE::BLINK_SLOW){
            setByProfile();
        }else{
            //so it aligns to the currently blinking leds on the next status switch
            refrenceBlinkingStatus ? turnOn() : turnOff();    
            if (isOn()){
                setDeadlineMillisecs(ledProfiles[activeProfile].blinkingRule->onTimeMs);
            }else{
                setDeadlineMillisecs(ledProfiles[activeProfile].blinkingRule->offTimeMs);
            }            
        }
    }
}

LED_PROFILE Led::getProfile() const
{
    return activeProfile;
}

void Led::setByProfile()
{
    ESP_LOGD(LED_TAG, "led set by profile: %d", activeProfile);  
    switch (activeProfile){
    //case BLINK_FAST:
    case BLINK_SLOW:
        ESP_LOGD(LED_TAG, "blink");    
        if (isOn()){
            setDeadlineMillisecs(ledProfiles[activeProfile].blinkingRule->offTimeMs);
            ESP_LOGD(LED_TAG, "turn LED OFF. Ovr is %d", (int)overrideMode);
            turnOff();
        }else{
            setDeadlineMillisecs(ledProfiles[activeProfile].blinkingRule->onTimeMs);
            ESP_LOGD(LED_TAG, "turn LED ON. Ovr is %d", (int)overrideMode);
            turnOn();
        }
        break;   
    case ALWAYS_OFF:
        setDeadlineMillisecs(DEADLINE_INF);
        if (isOn()){
            turnOff();  
        }
        break;
    case ALWAYS_ON: 
        setDeadlineMillisecs(DEADLINE_INF);
        if (!isOn()){
            turnOn();
        }
        break;
    default:
        ESP_LOGE(LED_TAG, "Unknown led profile");
        break;
    }
}

void Led::turnOn()
{
    logicalStatus = true;
    if (!overrideMode){
        gpio_->setOutputLevel(0);
    }  
}

void Led::turnOff()
{
    logicalStatus = false;
    if (!overrideMode){
        gpio_->setOutputLevel(1);
    }
}

bool Led::isOn() const
{
    return logicalStatus;
}

bool Led::isBlinking() const
{
    return activeProfile == LED_PROFILE::BLINK_SLOW;
}

bool Led::isGpioOn() const
{
    int level = gpio_->getOutputLevel();
    ESP_LOGI(LED_TAG, "is led on ? gpio num.: %d, led gpio level: %d", (int)gpio_->getNumber(), level);
    return (bool)level ? false : true; 
}

void Led::overrideStatus(const OverrideKey& key, bool overrideOn, bool status)
{
    (void)key;
    overrideMode = overrideOn;
    if (overrideMode){
        overrideMode = true;
        gpio_->setOutputLevel((int)!status);
    }else{
        overrideMode = false;
    }
}
