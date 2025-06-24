
#include "debug.h"

#include "esp_system.h"
#include "esp_heap_caps.h"
#include <soc/rtc.h>
#include <esp_clk_tree.h>
#include <esp_wifi.h>

// MALLOC_CAP_8BIT
// MALLOC_CAP_32BIT
// MALLOC_CAP_DMA
// MALLOC_CAP_INTERNAL
// MALLOC_CAP_SPIRAM
// MALLOC_CAP_INVALID
void checkMemory() {
    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t external_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(RAM_TAG, "Internal RAM free size: %d bytes\n", internal_free);
    ESP_LOGI(RAM_TAG, "External RAM (SPIRAM) free size: %d bytes\n", external_free);
}

void setLogLevels()
{
    //default: log everything
    esp_log_level_set("*", ESP_LOG_NONE); 
    #if CSV_LOG
        esp_log_level_set(CSV_TAG, ESP_LOG_DEBUG);
    #else
    esp_log_level_set(MAIN_TAG, ESP_LOG_DEBUG);  //ESP_LOG_WARN
    esp_log_level_set(SCHED_TAG, ESP_LOG_NONE); //ESP_LOG_DEBUG
    esp_log_level_set(BLE_TAG, ESP_LOG_NONE);  
    esp_log_level_set(I2C_POSIFA_TAG, ESP_LOG_NONE); 
    esp_log_level_set(I2C_DISPLAY_TAG, ESP_LOG_INFO); 
    esp_log_level_set(I2C_TAG, ESP_LOG_NONE); 
    esp_log_level_set(BUTTON_TAG, ESP_LOG_NONE);
    esp_log_level_set(FLASH_TAG, ESP_LOG_NONE); 
    esp_log_level_set(RAM_TAG, ESP_LOG_NONE);
    esp_log_level_set(BATTERY_TAG, ESP_LOG_NONE);
    esp_log_level_set(NTC_TAG, ESP_LOG_NONE); 
    esp_log_level_set(LED_TAG, ESP_LOG_NONE);
    esp_log_level_set(GPIO_TAG, ESP_LOG_NONE);  
    esp_log_level_set(CPU_FREQUENCY_TAG, ESP_LOG_NONE);  
    esp_log_level_set(COM_TAG, ESP_LOG_NONE);  
    //
    esp_log_level_set(DATALOGGER_SCREEN_TAG, ESP_LOG_WARN);

    //Nimble 
    esp_log_level_set("NimBLE", ESP_LOG_NONE);
    esp_log_level_set("NimBLEAdvertising", ESP_LOG_NONE);
    esp_log_level_set("NimBLEScan", ESP_LOG_NONE);
    esp_log_level_set("NimBLEHost", ESP_LOG_NONE);
    esp_log_level_set("NimBLEDevice", ESP_LOG_NONE);
    #endif
}

void logCPUfrequency()
{
    // Define the clock source to get the frequency for
    soc_module_clk_t clk_source = SOC_MOD_CLK_CPU;

    // Get the CPU frequency in Hz
    uint32_t cpu_freq_hz = 0;
    esp_err_t result = esp_clk_tree_src_get_freq_hz(clk_source, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &cpu_freq_hz);

    if (result == ESP_OK) {
        ESP_LOGI(CPU_FREQUENCY_TAG, "Current CPU frequency: %u Hz", (unsigned int)cpu_freq_hz);
    } else {
        ESP_LOGE(CPU_FREQUENCY_TAG, "Failed to get CPU frequency, error code: %d", result);
    }  
}

extern void testXtal()
{
    rtc_slow_freq_t freq = rtc_clk_slow_freq_get(); //careful, takes time!
    if (freq == RTC_SLOW_FREQ_32K_XTAL) {
        printf("Using external 32kHz crystal for RTC\n");
    } else {
        printf("Not using external 32kHz crystal for RTC\n");
    }
}


void check_wifi_mode() 
{ 
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err == ESP_OK) {
        switch (mode) {
            case WIFI_MODE_NULL:
                ESP_LOGI(MAIN_TAG, "Wi-Fi is OFF.");
                break;
            case WIFI_MODE_STA:
                ESP_LOGI(MAIN_TAG, "Wi-Fi is in STATION mode.");
                break;
            case WIFI_MODE_AP:
                ESP_LOGI(MAIN_TAG, "Wi-Fi is in ACCESS POINT mode.");
                break;
            case WIFI_MODE_APSTA:
                ESP_LOGI(MAIN_TAG, "Wi-Fi is in STATION+AP mode.");
                break;
            default:
                ESP_LOGI(MAIN_TAG, "Unknown Wi-Fi mode.");
        }
    } else {
        ESP_LOGE(MAIN_TAG, "Failed to get Wi-Fi mode: %s", esp_err_to_name(err));
    }
}


