/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "util.h"
//flash
#include "nvs_flash.h"
//// BLE 
//gattc
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
//fede
#include "host/ble_uuid.h"
//
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "ble_cts_cent.h"
#include "services/cts/ble_svc_cts.h"
//local ble
#include "ble-cts-central.h"
//task
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//led
#include "driver/gpio.h"
//serial
 #include "serial-module.h"

//----------------------------------------------------------------------------------

void task_main(void *pvParameters) 
{
    esp_log_level_set("*", ESP_LOG_NONE);
    esp_log_level_set(BLE_TAG, ESP_LOG_DEBUG); 
    ESP_LOGI(MAIN_TAG, "Entering main");
    int rc;

    // serial line
    SerialLine.begin();
    SerialLine.setWriteCallback(onSerialData);
    SerialLine.write("Serial Ready!\n");

    // Initialize NVS — it is used to store PHY calibration data
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_TAG, "Failed to init nimble %d", ret);
        return;
    }

    // Configure the host.
    ble_hs_cfg.reset_cb = ble_cts_cent_on_reset;
    ble_hs_cfg.sync_cb = ble_cts_cent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // Initialize data structures to track connected peers.
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    // Set the default device name.
    rc = ble_svc_gap_device_name_set("nimble-cts-cent");
    assert(rc == 0);

    // XXX Need to have template for store
    // TODO
    //ble_store_config_init(); // TODO: add this back

    nimble_port_freertos_init(ble_cts_cent_host_task);

    // Main loop with 1 second delay
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 ms (1 second)
    }
    ESP_LOGE(MAIN_TAG, "Leaving main");
}

extern "C" void app_main(void) 
{
    xTaskCreate(task_main, "task_main", 4096, NULL, 5, NULL);
}
