#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H
#pragma once

template <typename T>
inline T map_value(T value, T from_low, T from_high, T to_low, T to_high) {
    return (value - from_low) * (to_high - to_low) / (from_high - from_low) + to_low;
}

#endif