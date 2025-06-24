#pragma once

#include <cstdint>
#include <string>

#define DEADLINE_INF -1

class Counter
{
public:
    Counter() = default;
    ~Counter() = default;
    virtual void initCounter();

    virtual bool updateTimer(int rtosMinTime);
    void restartCounter();
    int32_t getDeadlineMilliseconds() const;
    virtual int32_t getRealTimeToDeadlineMilliseconds() const;
    int32_t getPassedTimeMilliseconds() const;
    void copyTimestampsTo(Counter* target) const;
    virtual std::string getLogCounter(std::string id) const;

protected:
    void setDeadlineMillisecs(int32_t milliseconds);
    
private:
    int64_t deadline_ms = 0;
    int64_t start_time = 0;
    int64_t current_time = 0;

};
