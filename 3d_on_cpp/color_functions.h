#ifndef COLOR_FUNCTIONS_H
#define COLOR_FUNCTIONS_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

inline glm::vec3 rainbow_function(const int _x) {
    float x = static_cast<float>(_x) / 1023.0f * 3.0f * glm::pi<float>() / 2.0f;
    float r, g, b;

    if (x <= glm::pi<float>()) r = std::sin(x + glm::pi<float>() / 2.0f);
    else r = std::sin(x + glm::pi<float>());

    g = std::sin(x);
    b = std::sin(x + 3.0f * glm::pi<float>() / 2.0f);

    return glm::vec3(std::max(r, 0.0f), std::max(g, 0.0f), std::max(b, 0.0f));
}

inline glm::vec3 get_inverted_color(const glm::vec3& color) {
    return glm::vec3(1.0f) - color;
}

#endif