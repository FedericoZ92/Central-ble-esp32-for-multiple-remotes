#pragma once

#include "bluetooth/i-ble-propagator.h"
#include "bluetooth/i-ble-listener.h"
#include <set>
//st #include "bluetooth/ble-defs.h"
//st
#include "g_include.h"


class BlePropagator : public IBlePropagator {
public:
    BlePropagator() = default;
    virtual ~BlePropagator() = default;

    //The subscription management methods.
    void attachBleListener(IBleListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) override;
    void detachBleListener(IBleListener *listener) override;
    void fireBleConnectionStatus(eInfoBleStatus value) override;

#ifdef VACUUM_TYPE   
    void fireCollectLatestAnalysisSector(uEventSectorType* sector, uint16_t sectorId) override;
    void fireCollectFullDatalogPartitionSector(uEventSectorType* sector, uint16_t sectorId) override;
    void fireLinuxEpoch(time_t epoch) override;
    void fireAskEraseFlashDatalogFromBle(void) override;
    void fireStartRecordingFromBle(void) override;
    void fireEndRecordingFromBle(void) override;
#endif
    void fireSetParameterFromBle(ParameterValueKey_t key, ParameterValueType_t value) override;
    void fireSetPcomIdParameterFromBle(PcomId* pcomId) override;

    void howManyBleListeners();
    bool isConnected() const;

protected:
    eInfoBleStatus connectionStatus = INFO_BLE_INACTIVE;

private:
    std::multiset<std::pair<PRIORITY, IBleListener*>, std::greater<>> ms;

};