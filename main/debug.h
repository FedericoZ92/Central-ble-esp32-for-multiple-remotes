#pragma once

#include <esp_log.h>
#include "esp_heap_caps.h"


#if defined(CONFIG_VACUUM_TYPE)
    #pragma message("Building in VACUUM mode")
    #define V(X) X // V for vacuum 
    #define VT(X,Y) X
#elif defined(CONFIG_TEMPERATURE_TYPE)
    #pragma message("Building in TEMPERATURE mode")
    #define V(X) /*X*/
    #define VT(X,Y) Y
#else
    #error "Neither CONFIG_VACUUM_TYPE nor CONFIG_TEMPERATURE_TYPE is defined"
#endif

#define TEST_DATALOGGER_AND_FLASH_DATALOG 1
#define TEST_CLEAR_FLASH_DATALOG 0
#define TEST_CLEAR_FLASH_NVS 0
#define TEST_DO_A_CALIBRATION 0
#define TEST_POSIFA 1

//TEST_ADC
#define TEST_ADC 0
#define VACUUM_MODE_NTC_PERIOD_MS_TEST 10000 //10s

//POVG02_DEBUG_MODE:
#define POVG02_DEBUG_MODE 1
// - enables debug menus
// - increased standby period before full shutdown
#define VALUE_OF_PERIOD_IN_STANDBY_BEFORE_SHUTDOWN_MS_TEST 1200000 //20min
// - time display stays on before standby
#define VALUE_OF_DISPLAY_ON_PERIOD_BEFORE_STANDBY_MS_TEST 60000 //1min 


#define CSV_LOG 0 // if ON, no other log 

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

