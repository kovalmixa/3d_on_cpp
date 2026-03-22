#ifndef COLOR_FUNCTIONS_H
#define COLOR_FUNCTIONS_H
#pragma once

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

inline sf::Color glm_sf_col(const glm::vec4& color) {
    return sf::Color(
        static_cast<uint8_t>(color.r * 255),
        static_cast<uint8_t>(color.g * 255),
        static_cast<uint8_t>(color.b * 255),
        static_cast<uint8_t>(color.a * 255)
    );
}

inline glm::vec4 sf_glm_col(const sf::Color& color) {
    return glm::vec4(
        static_cast<float>(color.r) / 255.0f,
        static_cast<float>(color.g) / 255.0f,
        static_cast<float>(color.b) / 255.0f,
        static_cast<float>(color.a) / 255.0f
    );
}

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