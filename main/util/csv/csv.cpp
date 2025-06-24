/*#include <sstream>
#include <iostream>
#include <iomanip>
#include "util/misc.h"
#include "util/defs.h"*/

#include "util/csv/csv.h"

Csv::Csv()
{
    setDeadlineMillisecs(300);
}

void Csv::logOnTerminal() const
{
    std::string text = "";
    for (auto &&s : csvSlots){
        text += s;
        text += ";";
    }
    ESP_LOGI(CSV_TAG, "%s", text.c_str());
}

//posifa
void Csv::updatePosifaReadings(uint8_t* message, uint8_t size)
{
    csvSlots[0] = std::to_string(getRawReading());
}

void Csv::updatePosifaMicronAndEvaporationCelsius(uint32_t voi, double evaporationTemperature)
{
    Pressure voidMeasure = getPosifaVoid(); //micron
    csvSlots[1] = voidMeasure.toStringNoUdm(); 
    Temperature evapMeasure = getEvaporationTemperature();
    csvSlots[3] = evapMeasure.toStringNoUdm();
    csvSlots[4] = calcDeltaTemperatureStr(); // depends on both evap and ntc
}

void Csv::updateCalibrationDone(const CalibrationPoint& point)
{
    // nothing
}

void Csv::updatePosifaSensorTemperature(PosifaCalibrationValueType_t rawTemperature, const CalibrationPoint_t& point) 
{
    // nothing
}

void Csv::updateSensorTestFeedback(bool success)
{
    (void)success;
    // nothing
}

//ntc
void Csv::updateNtcReading(double message)
{
    Temperature ntc = getNtcCelsiusTemperature();
    csvSlots[2] = ntc.toStringNoUdm(); 
    csvSlots[4] = calcDeltaTemperatureStr(); // depends on both evap and ntc   
}

std::string Csv::calcDeltaTemperatureStr() const
{
    Temperature deltaTemperature = Temperature(getNtcCelsiusTemperature().getValue() - getEvaporationTemperature().getValue(), TemperatureUdm_t::UDM_oC);
    return deltaTemperature.toStringNoUdm();
}