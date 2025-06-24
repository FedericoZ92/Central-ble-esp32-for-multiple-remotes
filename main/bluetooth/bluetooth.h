#pragma once

#include <nimble/nimble_port.h> 
#include <esp_nimble_hci.h>
#include <host/ble_hs.h>
#include <string.h>


class Bluetooth : public BlePropagator, 
{
public:
    Bluetooth() = default;
    ~Bluetooth() = default;

    void init();
    void suspendBleOperations(void); 

    // posifa
    virtual void updatePosifaReadings(uint8_t* message, uint8_t size) override;
    virtual void updatePosifaMicronAndEvaporationCelsius(uint32_t voi, double evaporationTemperature) override;
    virtual void updateCalibrationDone(const CalibrationPoint& point) override;
    virtual void updatePosifaSensorTemperature(PosifaCalibrationValueType_t rawTemperature, const CalibrationPoint_t& point) override;
    void updateSensorTestFeedback(bool success) override;

    // battery
    virtual void updateBatteryVoltage(double message) override;
    virtual void updateBatteryStatus(BATTERY_STATUS value) override;
    virtual void updateBatteryPercentage(uint8_t value) override;

    //ntc
    virtual void updateNtcReading(double message) override;

    //mode
    virtual void updateMode(MODE_TYPE value);

    //Flash
    virtual void updateNewFlashParameter(ParameterValueKey_t key, ParameterValueType_t value) override;
    virtual void updateNewDatalogPercentage(uint8_t pct) override;

    static uint16_t connHandle; 

private:
    static void Check_Ble_Notify_task(void* arg);
};

extern Bluetooth bluetooth;





