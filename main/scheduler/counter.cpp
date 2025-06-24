
#include <sstream>
#include <esp_timer.h>
#include "scheduler/counter.h"
#include "debug.h"
#include "scheduler/defs.h"

void Counter::initCounter()
{
    restartCounter();
}

bool Counter::updateTimer(int rtosMinTime)
{
    current_time = esp_timer_get_time();
    if (deadline_ms > 0){
        if ((current_time - start_time)/1000 > deadline_ms - rtosMinTime){
            start_time = current_time;
            return true;
        }
    }
    return false;
}

void Counter::setDeadlineMillisecs(int32_t milliseconds)
{
    ESP_LOGV(MAIN_TAG, "set deadline: %" PRId32, milliseconds);
    deadline_ms = milliseconds;
}

void Counter::restartCounter()
{
    start_time = esp_timer_get_time();
}

int32_t Counter::getDeadlineMilliseconds() const
{
    if (deadline_ms <= 0){
        return MAX_SCHEDULER_DELAY;
    }
    return deadline_ms;
}

int32_t Counter::getRealTimeToDeadlineMilliseconds() const
{
    if (deadline_ms < 0){
        return MAX_SCHEDULER_DELAY;
    }
    return deadline_ms - (esp_timer_get_time() - start_time)/1000;
}

int32_t Counter::getPassedTimeMilliseconds() const
{
    return (current_time - start_time)/1000; // use latest current time
}

void Counter::copyTimestampsTo(Counter* target) const
{
    if (target) {
        target->deadline_ms = this->deadline_ms;
        target->start_time = this->start_time;
        target->current_time = this->current_time;
    }
}

std::string Counter::getLogCounter(std::string id) const
{
    std::stringstream ss;
    ss << id << ": start time " << static_cast<int32_t>(start_time / 1000)
       << ", time to deadline ms: " << static_cast<int32_t>(getRealTimeToDeadlineMilliseconds())
       << ", deadline ms: " << static_cast<int32_t>(getDeadlineMilliseconds())
       << ", current time ms: " << static_cast<int32_t>(current_time / 1000) << "\n";

    return ss.str();
}

