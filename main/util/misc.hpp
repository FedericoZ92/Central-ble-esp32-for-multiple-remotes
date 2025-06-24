#pragma once

#include <cmath>

template <typename T>
T roundDecs(T value, int decimalDigits)
{
    // decimalDigits=1 -> 0.1, 1 decimal
    // decimalDigits=0 -> no decimals
    // decimalDigits=-1 -> round to nearest ten
    if (decimalDigits < 0){
        double multiplier = std::pow(10.0, decimalDigits*(-1));
        return std::round(value/multiplier)*multiplier;
    }
    double multiplier = 1;
    for (int i=decimalDigits; i>0; i--)
        multiplier *= 10;
    double multipliedValue = value * multiplier;
    double delta = multipliedValue - floor(multipliedValue);
    double ret = floor(multipliedValue);
    if (delta>=0.5)
        ret += 1;
    ret = ret/multiplier;
    return ret;
}

template <typename T>
T rescale(T value, T old_min, T old_max, T new_min, T new_max) 
{
    // Clamp input to the source range
    value = std::min(std::max(value, old_min), old_max);
    // Avoid division by zero
    if (old_max == old_min) {
        return new_min; // or throw, or return error value
    }
    // Calculate ranges
    T old_range = old_max - old_min;
    T new_range = new_max - new_min;
    // Rescale the value
    T scaled_value = ((value - old_min) * new_range / old_range) + new_min;
    return scaled_value;
}
