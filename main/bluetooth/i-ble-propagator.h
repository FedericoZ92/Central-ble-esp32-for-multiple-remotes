#pragma once

#include <iostream>
#include <string>
#include "bluetooth/i-ble-listener.h"
#include "util/listener-priority.h"
#include "g_include.h"

class IBlePropagator 
{
public:
    virtual void attachBleListener(IBleListener *listener, PRIORITY priority = PRIORITY_VERY_LOW) = 0;
    virtual void detachBleListener(IBleListener *listener) = 0;
    virtual void fireBleConnectionStatus(eInfoBleStatus value) = 0;

#ifdef VACUUM_TYPE   
    virtual void fireCollectLatestAnalysisSector(uEventSectorType* parts, uint16_t sector) = 0;
    virtual void fireCollectFullDatalogPartitionSector(uEventSectorType* parts, uint16_t sector) = 0;
    virtual void fireLinuxEpoch(time_t epoch) = 0;
    virtual void fireAskEraseFlashDatalogFromBle(void) = 0;
    virtual void fireStartRecordingFromBle(void) = 0;
    virtual void fireEndRecordingFromBle(void) = 0;
#endif
    virtual void fireSetParameterFromBle(ParameterValueKey_t key, ParameterValueType_t value) = 0;
    virtual void fireSetPcomIdParameterFromBle(PcomId* pcomId) = 0;

};

