#pragma once

#include <iostream>
#include "flash/nvs/parameter-key.h"
#include "flash/nvs/parameter-type.h"
#include "flash/nvs/pcom-id/pcom-id.h"
#include "g_include.h"

class IBleListener
{
public:
    IBleListener() = default;
    virtual ~IBleListener() = default;
    virtual void updateBleConnectionStatus(eInfoBleStatus value) = 0;
    virtual void updateCollectLatestAnalysisSector(uEventSectorType* sector, uint16_t sectorId) = 0;
    virtual void updateCollectFullDatalogPartitionSector(uEventSectorType* sector, uint16_t sectorId) = 0;
    virtual void updateAskEraseFlashDatalogFromBle(void) = 0;
    virtual void updateLinuxEpoch(time_t epoch) = 0;
    virtual void updateStartRecordingFromBle(void) = 0;
    virtual void updateEndRecordingFromBle(void) = 0;
    virtual void updateSetParameterFromBle(ParameterValueKey_t key, ParameterValueType_t value) = 0;
    virtual void updateSetPcomIdParameterFromBle(PcomId* pcomId) = 0;

};
