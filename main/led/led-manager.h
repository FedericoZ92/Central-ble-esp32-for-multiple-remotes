#pragma once


#include "led/led.h"
#include "working-mode/mode-listener.h"
#include "bluetooth/ble-listener.h"
#include "adc/battery/battery-listener.h"
#include "gpios/gpio-manager.h"
#include "screen-manager/screen-manager-listener.h"
//st
#include "g_include.h"


class LedManager : public Counter, 
                   public BleListener
{
public:
    LedManager(GpioManager* gpioManager);
    ~LedManager() = default;
    void init();

    bool updateLedTimersAndSetByMode(int rtosMinTime);

    //counter override
    bool updateTimer(int rtosMinTime) override;
    int32_t getRealTimeToDeadlineMilliseconds() const override;
    std::string getLogCounter(std::string id) const override;
    void initCounter() override;
    void setProfile(Led* led, LED_PROFILE profile);

private:
    GpioManager* gpios;
    std::vector<Led*> leds;
    Led rgbRedLed;
    Led rgbGreenLed; 
    Led rgbBlueLed;
    Led whiteLed;
    Led greenLed; 

};


