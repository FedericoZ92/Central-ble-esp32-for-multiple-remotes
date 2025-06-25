#pragma once

#include <esp_log.h>
#include "esp_heap_caps.h"


#define MAIN_TAG "Main"
#define SCHED_TAG "Sched."
#define BLE_TAG "F.Ble"
#define CSV_TAG "CSV"
#define I2C_POSIFA_TAG "I2C P."
#define I2C_DISPLAY_TAG "I2C D."
#define I2C_TAG "I2C"
#define BUTTON_TAG "Button"
#define FLASH_TAG "Flash"
#define RAM_TAG "RAM"
#define BATTERY_TAG "Battery"
#define NTC_TAG "NTC"
#define LED_TAG "LED"
#define GPIO_TAG "GPIO"
#define CPU_FREQUENCY_TAG "CPU_freq"
#define COM_TAG "COM"
#define DATALOGGER_SCREEN_TAG "DL"


extern void checkMemory();
extern void setLogLevels();
extern void logCPUfrequency();
extern void testXtal();
extern void check_wifi_mode();

