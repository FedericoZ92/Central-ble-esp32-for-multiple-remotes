
#pragma once

#include <iostream>
#include <string>

#include "posifa/posifa-listener.h"
#include "i2c/i2c-bus.h"
#include "gpios/gpios.h"

#include "SSD1305_OLED.h"


#define DISPLAY_SDA I2C_DISP_SDA 
#define DISPLAY_SCL I2C_DISP_SCL
#define DISPLAY_I2C_ADDR 0x3D


class ScreenDriver: public SSD1305 //, public PosifaListener 
{
public:
    // Constructor
    ScreenDriver(I2C& i2c);
    ~ScreenDriver();
    
    //void updatePosifaVoid(uint16_t value);

    void OLED_I2C_OFF(void) override;
	void OLED_I2C_ON(void) override;

    void I2C_Write_Byte(uint8_t value, uint8_t Cmd) override;
    void I2C_Write_Bytes(const uint8_t* value, uint8_t size, uint8_t Cmd) override;

private:
    I2C* i2c_;

};