#pragma once

#include "esp_attr.h"
#include "debug.h"

#define ON_ENA GPIO_NUM_8
#define ON_POSIFA GPIO_NUM_7 //was GPIO_NUM_20 // (NEW)
#define ON_SW GPIO_NUM_18

#define I2C_MASTER_SCL_IO GPIO_NUM_36    // GPIO number for I2C SCL
#define I2C_MASTER_SDA_IO GPIO_NUM_35    // GPIO number for I2C SDA
#define POSIFA_SCL I2C_MASTER_SCL_IO
#define POSIFA_SDA I2C_MASTER_SDA_IO

#define I2C_DISP_SDA GPIO_NUM_47
#define I2C_DISP_SCL GPIO_NUM_48

#define DISP_RST GPIO_NUM_40 // was GPIO_NUM_4 // (NEW)
#define DISP_BKL_EN GPIO_NUM_17
#define DISP_CS GPIO_NUM_42 // was GPIO_NUM_13 // (NEW)

#define SCROLL_BUTTON_GPIO GPIO_NUM_20 //GPIO_NUM_46
#define ON_SW_BUTTON GPIO_NUM_18

#define VREF_REAL VT(adc1_channel_t(0), ADC1_CHANNEL_6)
#define NTC_BOARD VT(ADC1_CHANNEL_1, gpio_num_t(0)) //GPIO2, ntc
#define VIN_MEAS VT(ADC1_CHANNEL_0, ADC1_CHANNEL_4)
#define VIN_CHECK VT(GPIO_NUM_39, GPIO_NUM_19) // to allow/unallow battery reading

#define IRT_DATA GPIO_NUM_41
#define IRT_CLK GPIO_NUM_42
#define IRT_MEAS GPIO_NUM_1

#define HW_REV GPIO_NUM_19 
#define LED_RGB_R VT(GPIO_NUM_4, GPIO_NUM_39)
#define LED_RGB_G VT(GPIO_NUM_5, GPIO_NUM_38) 
#define LED_RGB_B VT(GPIO_NUM_6, GPIO_NUM_41)  
#define LED_W VT(GPIO_NUM_3, GPIO_NUM_40)
#define LED_G VT(GPIO_NUM_41, gpio_num_t(0)) //unused in temperature
#define NTC_CHECK GPIO_NUM_20 // per sonda di temperatura
#define AIN_CHECK GPIO_NUM_38 //to allow/unallow ntc check 

//flash
#define FLASH_SPI_CS GPIO_NUM_10 // Alto per non alimentare flash (light sleep)

//Adc
#define ADC_WIDTH  ADC_WIDTH_BIT_12 // 12-bit width resolution (values range from 0 to 4095)
#define ADC_ATTEN  ADC_ATTEN_DB_11  // Attenuation to measure higher voltages (0 - 3.3V)
#define ADC_ATTEN_BATT_TEMP_SENSOR ADC_ATTEN_DB_6
#define ADC_ATTEN_VREF ADC_ATTEN_DB_6

#define NTC_EXT ADC1_CHANNEL_5 // per sonda di temperatura

//Non ancora usati
#define FLASH_SPI_HOLD GPIO_NUM_9
#define FLASH_SPI_DI GPIO_NUM_11
#define FLASH_SPI_CLK GPIO_NUM_12
#define FLASH_DO GPIO_NUM_13
#define FLASH_WP GPIO_NUM_14
#define IO0 GPIO_NUM_0
#define ADC_DRDY GPIO_NUM_37
#define BUZZER_PWM GPIO_NUM_38



