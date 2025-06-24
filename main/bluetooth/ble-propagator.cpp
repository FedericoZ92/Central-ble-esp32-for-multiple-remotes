
#include "debug.h"
#include "bluetooth/ble-propagator.h"
#include <functional>

void BlePropagator::attachBleListener(IBleListener *listener, PRIORITY priority)
{
    ms.emplace(priority, listener);
}

void BlePropagator::detachBleListener(IBleListener *listener) 
{
    auto it = std::find_if(ms.begin(), ms.end(), [listener](const std::pair<PRIORITY, IBleListener*>& pair) {
        return pair.second == listener;
    });

    if (it != ms.end()) {
        ms.erase(it); // Remove the item
    }
}

void BlePropagator::fireBleConnectionStatus(eInfoBleStatus value) 
{
    ESP_LOGI(BLE_TAG, "start fire ble status: %d", (int)value);
    connectionStatus = value;
    for (const auto& pair : ms) {
        ESP_LOGD(BLE_TAG, "fire ble status!");
        pair.second->updateBleConnectionStatus(value);
    }
}

#ifdef VACUUM_TYPE   
void BlePropagator::fireCollectLatestAnalysisSector(uEventSectorType* sector, uint16_t sectorId)
{
    ESP_LOGI(COM_TAG, "start fire collect latest an. sector");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire collect analysis!");
        pair.second->updateCollectLatestAnalysisSector(sector, sectorId);
    }
    sectorIsReady = true;
}

void BlePropagator::fireCollectFullDatalogPartitionSector(uEventSectorType* sector, uint16_t sectorId)
{
    ESP_LOGI(COM_TAG, "start fire collect full mem. sector");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire collect analysis!");
        pair.second->updateCollectFullDatalogPartitionSector(sector, sectorId);
    }
    sectorIsReady = true;
}

void BlePropagator::fireAskEraseFlashDatalogFromBle(void)
{
    ESP_LOGI(BLE_TAG, "fire ask erase datalog from ble");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire ask erase datalog from ble!");
        pair.second->updateAskEraseFlashDatalogFromBle();
    }
}

void BlePropagator::fireLinuxEpoch(time_t epoch)
{
    ESP_LOGI(BLE_TAG, "start fire linnux epoch");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire collect analysis!");
        pair.second->updateLinuxEpoch(epoch);
    }
}

void BlePropagator::fireStartRecordingFromBle(void)
{
    ESP_LOGI(BLE_TAG, "start fire start recording from ble");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire start recording!");
        pair.second->updateStartRecordingFromBle();
    }
}

void BlePropagator::fireEndRecordingFromBle(void)
{
    ESP_LOGI(BLE_TAG, "start fire end recording from ble");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire end recording!");
        pair.second->updateEndRecordingFromBle();
    }
}
#endif

void BlePropagator::fireSetParameterFromBle(ParameterValueKey_t key, ParameterValueType_t value)
{
    ESP_LOGI(BLE_TAG, "set param from ble");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire set aram from ble!");
        pair.second->updateSetParameterFromBle(key, value);
    }
}

void BlePropagator::fireSetPcomIdParameterFromBle(PcomId* pcomId)
{
    ESP_LOGI(BLE_TAG, "set param from ble");
    for (const auto& pair : ms) {
        ESP_LOGI(BLE_TAG, "fire set aram from ble!");
        pair.second->updateSetPcomIdParameterFromBle(pcomId);
    }
}

bool BlePropagator::isConnected() const
{
    return connectionStatus == INFO_BLE_CONNECTED;
}

void BlePropagator::howManyBleListeners() {
    ESP_LOGI(MAIN_TAG, "There are %d ble listeners", (int)ms.size());
}



