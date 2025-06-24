#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include "debug.h"
#include "util/misc.h"
#include "util/defs.h"

int64_t getDurationUs(int64_t start, int64_t end)
{
    return end - start;
}

std::string uint16ToString(uint16_t value)
{
    return std::to_string(value);  
}

std::string doubleToString(double value, uint8_t decimals)
{
    if (std::isnan(value)) {
        return INVALID_VALUE_DEFAULT;
    }
    // Handle 0 explicitly
    if (value == 0.0) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(decimals) << 0.0;
        return out.str();
    }
    // Format the number
    std::ostringstream out;
    out << std::fixed << std::setprecision(decimals) << value;
    std::string result = out.str();
    return result;
}

std::string bytesToHexString(const unsigned char* bytes, size_t size) {
    std::ostringstream oss;
    for (size_t i = 0; i < size; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
        if (i < size - 1) {
            oss << " "; // Add a space between bytes for readability.
        }
    }
    return oss.str();
}    

uint16_t reconstruct2BigEndianBytes(const uint8_t* bytes)
{
    return static_cast<uint16_t>(static_cast<uint16_t>(bytes[0]) << 8 | bytes[1]);
}

uint16_t reconstruct2LittleEndianBytes(const uint8_t* bytes)
{
    return static_cast<uint16_t>(static_cast<uint16_t>(bytes[1]) << 8 | bytes[0]);
}

char getHexDigit(uint8_t nibble) 
{
    return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
}

char getFirstHexDigit(uint8_t byte)
{
    return getHexDigit((byte >> 4) & 0xF); 
}

char getSecondHexDigit(uint8_t byte)
{
    return getHexDigit(byte & 0xF);       
}

time_t getLinuxEpoch()
{
    time_t now;
    time(&now);
    return now;
}

bool setLinuxEpoch(time_t newEpoch)
{
    timeval tv;
    tv.tv_sec = newEpoch;
    tv.tv_usec = 0; // optional microseconds

    return (settimeofday(&tv, nullptr) == 0) ? true : false;
}

void setDateTime(int year, int month, int day, int hour, int minute, int second)
{
    struct tm timeinfo;
    timeinfo.tm_year = year - 1900; // tm_year is years since 1900
    timeinfo.tm_mon = month - 1;   // tm_mon is 0-based
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;

    time_t t = mktime(&timeinfo); // Convert to Unix epoch
    if (t != -1) {
        struct timeval tv;
        tv.tv_sec = t;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL); // Set the system time
        ESP_LOGD(MAIN_TAG, "Time updated to: %s", ctime(&t));
    } else {
        ESP_LOGD(MAIN_TAG, "Error: Invalid time provided");
    }
}

bool compareUint8Arrays(const uint8_t* first, const uint8_t* second, uint8_t size) 
{
    for (uint8_t i = 0; i < size; i++) {
        if (first[i] != second[i]) {
            return false;
        }
    }
    return true;
}

bool copyUint8Array(uint8_t* to, const uint8_t* from, uint8_t size) 
{
    if (!to || !from) {
        return false;  // Return false if either pointer is null
    }

    for (uint8_t i = 0; i < size; i++) {
        to[i] = from[i];
    }
    return true;  // Return true if the copy was successful
}

std::string getHexFormatByte(uint8_t decByte)
{
    std::stringstream ss;
    ss << " 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)decByte;
    return ss.str();
}

std::string getHexBytePairs(const uint8_t* readings, uint8_t number, ENDIANNESS endianness)
{
    std::stringstream ss;
    for (size_t i = 0; i < number; i += 2) {
        uint16_t value = endianness == ENDIANNESS_BIG 
                        ? reconstruct2BigEndianBytes(&readings[i])
                        : reconstruct2LittleEndianBytes(&readings[i]);
        ss << i / 2 << ":" << "0x" << std::setw(4) << value << "[";  
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)readings[i];
        if (i + 1 < number) {
            ss << ", 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)readings[i + 1];
        }
        ss << "] ";
    }
    return ss.str();
}

std::string getDecimalBytePairs(const uint8_t* readings, uint8_t number, ENDIANNESS endianness)
{
    std::stringstream ss;
    for (size_t i = 0; i < number; i += 2) {
        uint16_t value = endianness == ENDIANNESS_BIG 
                        ? reconstruct2BigEndianBytes(&readings[i])
                        : reconstruct2LittleEndianBytes(&readings[i]);
        ss << i / 2 << ":" << value << "[";
        ss << std::dec << (int)readings[i];  // Output in decimal format
        if (i + 1 < number) {
            ss << ", " << std::dec << (int)readings[i + 1];  // Output in decimal format
        }
        ss << "] ";
    }
    return ss.str();
}

std::string getHexSingleBytes(const uint8_t* readings, uint8_t number)
{
    std::stringstream ss;
    for (size_t i = 0; i < number; i += 1) {
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)readings[i] << ", ";
    }
    return ss.str();    
}

std::string getDecimalSingleBytes(const uint8_t* readings, uint8_t number)
{
    std::stringstream ss;
    for (size_t i = 0; i < number; i += 1) {
        ss << std::dec << (int)readings[i] << ", ";  // Output in decimal format
    }
    return ss.str();    
}

std::string stringFromChar(const char* ptr, size_t size) 
{
    return std::string(ptr, size);  // Handles non-null-terminated strings safely
}

uint8_t getLittleByte(uint16_t value)
{
    return static_cast<uint8_t>((value) & 0xff);
}

uint8_t getBigByte(uint16_t value)
{
    return static_cast<uint8_t>((value >> 8) & 0xff);
}

bool areDoublesEqual(double a, double b, double epsilon)
{
    return std::abs(a - b) < epsilon;
}
