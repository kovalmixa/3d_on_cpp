#ifndef RANDOM_FUNCTIONS_H
#define RANDOM_FUNCTIONS_H
#pragma once

#include <random>

template <typename T>
inline T random(T begin, T end) {
    static thread_local std::mt19937_64 engine(std::random_device{}());
    if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(begin, end);
        return dist(engine);
    }
    else {
        std::uniform_int_distribution<T> dist(begin, end);
        return dist(engine);
    }
}

#endif