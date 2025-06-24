#pragma once

#include <vector>
#include "gpios/gpio.h"
#include "gpios/gpios.h"
#include "gpios/output-gpio.h"
#include "working-mode/mode-listener.h"
#include "keyboard/keyboard-listener.h"
#include "util/thread-events/sender.h"


// Estendibile inserendo gestione eventi, pero ora usato solo per tracciare e proteggere (mutex) stato uscita
class GpioManager 
{
public:
    GpioManager() = default;
    ~GpioManager() = default;

    void configure();
    //
    void configure_ON_ENA() const;
    void enable_ON_ENA();
    void shutdown();

    void configure_posifa_i2c() const;
    void configure_ON_POSIFA() const;
    void enable_ON_POSIFA();
    void disable_ON_POSIFA();

    void configure_display() const;
    void enable_display();
    void disable_display();

    void configure_SCROLL_BUTTON_GPIO() const;
    void configure_ON_SW_BUTTON() const;

    void configure_vinCheck();
    void configure_ainOrNtcCheck();
    void allowAdcReading();
    void unallowAdcReading();
    void configure_battery() const;
    void configure_ntc() const;

    void configure_rgb_green_led() const;
    void configure_rgb_red_led() const;
    void configure_rgb_blue_led() const;
    void configure_white_led() const;
    void configure_green_led() const;
    void disable_rgb_green_led();
    void disable_rgb_red_led();
    void disable_rgb_blue_led();
    void disable_white_led();
    void disable_green_led();

    void configure_flash() const;
    void enable_flash();
    void disable_flash();

    void initOthers();
    //
    void add(GpioBase* gpio);

    GpioBase* get(gpio_num_t number);

private:
    std::vector<GpioBase*> gpios;
    GpioOutput gpioEna = GpioOutput(ON_ENA);
    GpioOutput gpioPosi = GpioOutput(ON_POSIFA);  
    GpioOutput gpioDispSda = GpioOutput(I2C_DISP_SDA);   
    GpioOutput gpioDispScl = GpioOutput(I2C_DISP_SCL);  
    GpioOutput gpioRst = GpioOutput(DISP_RST);  
    GpioOutput gpioCs = GpioOutput(DISP_CS);  
    GpioOutput gpioBkl = GpioOutput(DISP_BKL_EN);  
    GpioOutput gpioBatt = GpioOutput(VIN_CHECK);  
    GpioOutput gpioNtc = GpioOutput(VT(AIN_CHECK, NTC_CHECK));  
    GpioOutput gpioScroll = GpioOutput(SCROLL_BUTTON_GPIO);  
    GpioOutput gpioOnSw = GpioOutput(ON_SW_BUTTON); 
    GpioOutput gpioRgbLedGreen = GpioOutput(LED_RGB_G ); //rgb green
    GpioOutput gpioRgbLedRed = GpioOutput(LED_RGB_R ); //rgb red  
    GpioOutput gpioRgbLedBlue = GpioOutput(LED_RGB_B ); //rgb blue
    GpioOutput gpioLedWhite = GpioOutput(LED_W); //white
    GpioOutput gpioLedGreen = GpioOutput(LED_G); //green   
    GpioOutput gpioFlashChipSelect = GpioOutput(FLASH_SPI_CS);    
    GpioOutput gpioBuzzerPwm = GpioOutput(BUZZER_PWM); //buzzer    
 
    void configure_display_i2c() const;
    void configure_display_chip_select() const;
    void enable_display_chip_select();
    void disable_display_chip_select();
    void configure_display_backlight() const;
    void enable_display_backlight();
    void disable_display_backlight();
    void configure_display_reset() const;
    void enable_display_reset();
    void disable_display_reset();
};
    