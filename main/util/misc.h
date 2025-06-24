#pragma once

#include <stdint.h>
#include <string>
#include <ctime>

#define ASSERT_LESS_OR_EQUAL_INT(A, B) assert(((int)A) <= ((int)B))

enum ENDIANNESS{
    ENDIANNESS_BIG,
    ENDIANNESS_SMALL,
};

template <typename T> T roundDecs(T value, int decimalDigits);

int64_t getDurationUs(int64_t start, int64_t end);

template <typename T> T rescale(T value, T old_min, T old_max, T new_min, T new_max);

std::string uint16ToString(uint16_t value);

std::string doubleToString(double value, uint8_t decimals);

std::string bytesToHexString(const unsigned char* bytes, size_t size);

uint16_t reconstruct2BigEndianBytes(const uint8_t* bytes);
uint16_t reconstruct2LittleEndianBytes(const uint8_t* bytes);

char getHexDigit(uint8_t nibble);
char getFirstHexDigit(uint8_t byte);
char getSecondHexDigit(uint8_t byte);

time_t getLinuxEpoch();
bool setLinuxEpoch(time_t newEpoch);
void setDateTime(int year, int month, int day, int hour, int minute, int second);

bool compareUint8Arrays(const uint8_t* first, const uint8_t* second, uint8_t size); 
bool copyUint8Array(uint8_t* to, const uint8_t* from, uint8_t size);

std::string getHexFormatByte(uint8_t decByte);
std::string getHexBytePairs(const uint8_t* readings, uint8_t number, ENDIANNESS endianness);
std::string getDecimalBytePairs(const uint8_t* readings, uint8_t number, ENDIANNESS endianness);
std::string getHexSingleBytes(const uint8_t* readings, uint8_t number);
//example
//char s = '°';
//std::string str = getHexSingleBytes(reinterpret_cast<const uint8_t*>(&s), 1);
//ESP_LOGD(MAIN_TAG,"%s", str.c_str());
//
std::string getDecimalSingleBytes(const uint8_t* readings, uint8_t number);
std::string stringFromChar(const char* ptr, size_t size);

uint8_t getLittleByte(uint16_t value);
uint8_t getBigByte(uint16_t value);

bool areDoublesEqual(double a, double b, double epsilon = 1e-9);

#include "misc.hpp"