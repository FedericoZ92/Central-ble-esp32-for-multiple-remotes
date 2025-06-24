#include "bluetooth/bluetooth.h"
//log and debug
#include <iostream>
#include <cstdint>
#include <iomanip>
#include "debug.h"
// BLE stack
#include <nimble/ble.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <host/ble_hs.h>
#include <host/util/util.h>
#include <services/gap/ble_svc_gap.h>
#include <services/cts/ble_svc_cts.h>
#include <services/gatt/ble_svc_gatt.h>
// Functionalities
#include <nvs_flash.h>
#include <freertos/FreeRTOSConfig.h>
#include "util/misc.h"
//set power 3dB
#include <esp_bt.h>

//st
#include "g_include.h"
#include "flash/dictionary/flash-dictionary-listener.h"

Bluetooth bluetooth;

TaskHandle_t CHKBLE;

#ifdef VACUUM_TYPE   
void Bluetooth::Check_Ble_Notify_task(void* arg)
{
    sVacuum_Check_Ble_Notify_msg_type ble_notify_msg;
    uint8_t i;
    unit32AndByte uLongByte;
    Vacuum_Check_Ble_Notify_queue = xQueueCreate(10, sizeof(sVacuum_Check_Ble_Notify_msg_type));
    for(;;) 
    {
        if(xQueueReceive(Vacuum_Check_Ble_Notify_queue, &ble_notify_msg, portMAX_DELAY)) 
        {
            switch(ble_notify_msg.notifyMsg)
            {
                case INFO_BLE_STATUS:
                    bluetooth.fireBleConnectionStatus((eInfoBleStatus)ble_notify_msg.val[0]);
                    break;
                case NEW_UDM_T:
                    bluetooth.fireSetParameterFromBle(ParameterValueKey_t::PAR_UDM_T, (ParameterValueType_t)ble_notify_msg.val[0]);
                    break;
                case NEW_UDM_P:
                    bluetooth.fireSetParameterFromBle(ParameterValueKey_t::PAR_UDM_P, (ParameterValueType_t)ble_notify_msg.val[0]);
                    break;
                case NEW_INTERVAL:
                    bluetooth.fireSetParameterFromBle(ParameterValueKey_t::PAR_DL_TIME, (ParameterValueType_t)ble_notify_msg.val[0]);
                    break;
                case NEW_NAME:
                    PcomId id;
                    memcpy(id.name, ble_notify_msg.val, PCOM_ID_BYTE_SIZE);
                    bluetooth.fireSetPcomIdParameterFromBle(&id);
                    break;
                case LOGGER_START:
                    for(i = 0; i < 4; i++)
                        uLongByte.bVal[i] = ble_notify_msg.val[i];
                    bluetooth.fireLinuxEpoch(uLongByte.valInt);
                    bluetooth.fireStartRecordingFromBle();
                    break;
                case LOGGER_STOP:
                    bluetooth.fireEndRecordingFromBle();
                    break;
                case LOGGER_ALL:
                    bluetooth.fireCollectFullDatalogPartitionSector(&loggerData, ble_notify_msg.val[0]);
                    break;
                case LOGGER_LAST:
                    bluetooth.fireCollectLatestAnalysisSector(&loggerData, ble_notify_msg.val[0]);
                    break;
                case LOGGER_RESET:
                    bluetooth.fireAskEraseFlashDatalogFromBle();
                    break;
                case GET_ABS_EVENT_COUNT:
                    absEventCount = bluetooth.getDatalogPartitionEventCount();
                    counterIsReady = true;
                    break;
                case GET_LAST_EVENT_COUNT:
                    lastSessionEventCount = bluetooth.getLatestRegistrationEventCount();
                    counterIsReady = true;
                    break;
                default:
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}
#endif

    void Bluetooth::init()
{
    #ifdef VACUUM_TYPE   
    UpdVacuumStatus(V_IDX_ST_VERSION, (uint16_t)getNvsVersion());
    UpdVacuumStatus(V_IDX_ST_UDMT, (TemperatureUdm_t(getNvsTemperatureUdm())).toUint());
    UpdVacuumStatus(V_IDX_ST_UDMP, (PressureUdm_t(getNvsPressureUdm()).toUint()));
    UpdVacuumStatus(V_IDX_ST_INTERVAL, getNvsDataloggerInterval());

    UpdVacuumConfig(IDX_CFG_UDMT, (TemperatureUdm_t(getNvsTemperatureUdm())).toUint());
    UpdVacuumConfig(IDX_CFG_UDMP, (PressureUdm_t(getNvsPressureUdm()).toUint()));
    UpdVacuumConfig(IDX_CFG_INTERVAL, getNvsDataloggerInterval());
    UpdVacuumNameConfig(IDX_CFG_NAME, getNvsPcomId().name);
    xTaskCreate(Check_Ble_Notify_task, "Check_Ble_Notify_task",    4096, NULL, 6, &CHKBLE);
    #elif defined TEMPERATURE_TYPE
    #endif
}
// Function to pause BLE operations (stop advertising and cancel connections)
void Bluetooth::suspendBleOperations(void) {
    //st
    // ESP_LOGV(BLE_TAG, "Pausing BLE operations...");

    // // Stop advertising (if it's active)
    // ble_gap_adv_stop();
    // ESP_LOGV(BLE_TAG, "Pausing BLE operations... ble_gap_adv_stop");
    // // Cancel any ongoing connection procedures (if any)
    // if (ble_gap_conn_active()) {
    //     ble_gap_conn_cancel();
    //     ESP_LOGV(BLE_TAG, "Pausing BLE operations... ble_gap_conn_cancel");
    // }

    // vTaskDelay(pdMS_TO_TICKS(100));
    // // You can also stop scanning, if applicable (not needed for advertising)
    // nimble_port_stop();
    // ESP_LOGV(BLE_TAG, "Pausing BLE operations... nimble_port_stop");
    // nimble_port_freertos_deinit();
    // //nimble_port_deinit();  // This deinitializes the NimBLE stack
    // ESP_LOGV(BLE_TAG, "Pausing BLE operations... nimble_port_freertos_deinit");
    // esp_bt_controller_disable();
    // ESP_LOGV(BLE_TAG, "Pausing BLE operations... esp_bt_controller_disable");

    // bluetooth.fireBleConnectionStatus(BLE_INACTIVE);
}

void Bluetooth::updatePosifaReadings(uint8_t* messag, uint8_t size) 
{
    //do nothing
}

void Bluetooth::updatePosifaMicronAndEvaporationCelsius(uint32_t voi, double evaporationTemperature)
{
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_TEVA, (int16_t) (evaporationTemperature *10.0));
    #endif
    // ESP_LOGE(BLE_TAG, "tEva upd: %f",evaporationTemperature);
}

void Bluetooth::updateCalibrationDone(const CalibrationPoint& point)
{
    // do nothing
}

void Bluetooth::updatePosifaSensorTemperature(PosifaCalibrationValueType_t rawTemperature, const CalibrationPoint_t& point)
{
    // do nothing
}

void Bluetooth::updateSensorTestFeedback(bool success)
{
    (void)success;
    // do nothing
}

// battery
void Bluetooth::updateBatteryVoltage(double message)
{
    //TODO
}

void Bluetooth::updateBatteryStatus(BATTERY_STATUS value)
{
    //TODO
}

void Bluetooth::updateBatteryPercentage(uint8_t value)
{
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_BATTERY, (int16_t)value);
    #elif defined TEMPERATURE_TYPE
    #endif
    ESP_LOGE(BLE_TAG, "battery perc. upd: %d",value);
}

void Bluetooth::updateNtcReading(double message)
{
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_TAMB, (int16_t) (message *10.0));
    #elif defined TEMPERATURE_TYPE
    #endif
    ESP_LOGE(BLE_TAG, "tAmb upd: %f",message);
}

void Bluetooth::updateMode(MODE_TYPE value)
{
    // ble è input a mode manager per tutti gli altri stati a parte questo
    if (value == IMMINENT_SHUTDOWN){
        suspendBleOperations();
        ESP_LOGI(MAIN_TAG, "Ble, full shutdown");
    }
    switch (value)
    {
        case OPERATIVE:
        case OPERATIVE_CONNECTED:
        case SLEEPING_CONNECTED:
        case SLEEPING_DISCONNECTED:
        case IMMINENT_SHUTDOWN:     
            V(UpdVacuumStatus(V_IDX_ST_FLAGS, (int16_t)0));
            break;
        case OPERATIVE_RECORDING:
        case OPERATIVE_CONNECTED_RECORDING:
        case SLEEPING_CONNECTED_RECORDING:    
        case SLEEPING_DISCONNECTED_RECORDING:    
            V(UpdVacuumStatus(V_IDX_ST_FLAGS, (int16_t)1));
            break;     
        default:
            break;
    }
}

void Bluetooth::updateNewFlashParameter(ParameterValueKey_t key, ParameterValueType_t value)
{
    switch (key.toUint()) {
    case ParameterValueKey_t::PAR_STANDBY:
        // no need to manage
        break;  
    case ParameterValueKey_t::PAR_UDM_T:
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_UDMT, (int16_t) value);
        UpdVacuumConfig(IDX_CFG_UDMT, (int16_t) value);
    #elif defined TEMPERATURE_TYPE
    #endif
        ESP_LOGV(BLE_TAG, "Flash param UdmT: %d",(uint8_t)value);
        break;
    case ParameterValueKey_t::PAR_UDM_P:    
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_UDMP, (int16_t) value);
        UpdVacuumConfig(IDX_CFG_UDMP, (int16_t) value);
        ESP_LOGV(BLE_TAG, "Flash param UdmP: %d",(uint8_t)value);
    #endif
        break;
    case ParameterValueKey_t::PAR_PCOM_ID_4: // merely for debug    
    #ifdef VACUUM_TYPE   
        UpdVacuumNameConfig(IDX_CFG_NAME, getNvsPcomId().name);
    #elif defined TEMPERATURE_TYPE
    #endif
        ESP_LOGV(BLE_TAG, "Flash param Name: %s",getNvsPcomId().name);
        break; 
    }
}

void Bluetooth::updateNewDatalogPercentage(uint8_t pct)
{
    #ifdef VACUUM_TYPE   
        UpdVacuumStatus(V_IDX_ST_STORAGE, (int16_t)pct);
    #elif defined TEMPERATURE_TYPE
    #endif
    ESP_LOGE(BLE_TAG, "battery perc. upd: %d",pct);
}



