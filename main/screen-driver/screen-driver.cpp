
#include "screen-driver/screen-driver.h"
#include "debug.h"
#include "i2c/i2c-bus.h"
#include <esp_err.h>

ScreenDriver::ScreenDriver(I2C& i2c) : 
    SSD1305(),
    i2c_(&i2c)
{
    i2c.init(DISPLAY_SDA, DISPLAY_SCL, DISPLAY_I2C_ADDR, I2C_MASTER_FREQ_HZ);
    setRotation(0);
}

ScreenDriver::~ScreenDriver()
{
    i2c_ = nullptr;
}

void ScreenDriver::OLED_I2C_OFF(void)
{
    i2c_->close();
}

void ScreenDriver::OLED_I2C_ON(void)
{
    ESP_LOGI(I2C_DISPLAY_TAG, "OLED_I2C_ON...");
    ESP_ERROR_CHECK(i2c_->begin(DISPLAY_SDA , DISPLAY_SCL, I2C_MASTER_FREQ_HZ)); // ?
    ESP_LOGI(I2C_DISPLAY_TAG, "OLED_I2C_ON done");
}

void ScreenDriver::I2C_Write_Byte(uint8_t value, uint8_t Cmd)
{
    //uint8_t valueToSend = value;
    esp_err_t err = i2c_->writeByte(i2c_->getAddr(), Cmd, value); //Cmd is intended as the register
    ESP_LOGI(I2C_TAG, "write byte cmd: %d, err: %d", (int)value, (int)err); 
                    //uint8_t devAddr, uint8_t regAddr, const uint8_t *data, int32_t timeout = -1
}

void ScreenDriver::I2C_Write_Bytes(const uint8_t* value, uint8_t size, uint8_t Cmd)
{
    //uint8_t valueToSend = value;
    i2c_->writeBytes(i2c_->getAddr(), Cmd, size, value, -1); //Cmd is intended as the register
                    //uint8_t devAddr, uint8_t regAddr, const uint8_t *data, int32_t timeout = -1
}
