#pragma once

#include "posifa/dictionary/posifa-dictionary-listener.h"
#include "posifa/posifa-listener.h"
#include "adc/temperature/dictionary/ntc-dictionary-listener.h"
#include "adc/temperature/ntc-listener.h"
#include "scheduler/counter.h"

class Csv : public Counter, 
            public PosifaListener, 
            public PosifaDictionaryListener, 
            public NtcListener, 
            public NtcDictionaryListener
{
public:
    Csv();
    ~Csv() = default;

    void logOnTerminal() const;

    //posifa
    void updatePosifaReadings(uint8_t* message, uint8_t size) override;
    void updatePosifaMicronAndEvaporationCelsius(uint32_t voi, double evaporationTemperature) override;
    void updateCalibrationDone(const CalibrationPoint& point) override;
    void updatePosifaSensorTemperature(PosifaCalibrationValueType_t rawTemperature, const CalibrationPoint_t& point) override;
    void updateSensorTestFeedback(bool success) override;

    //ntc
    virtual void updateNtcReading(double message) override;


private:
    std::array<std::string, 5> csvSlots{};

    std::string calcDeltaTemperatureStr() const;
};