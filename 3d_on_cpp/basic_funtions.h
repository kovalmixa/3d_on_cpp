#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H
#pragma once

#include <algorithm>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <SFML/Graphics.hpp>
#include <random>
inline float deg_to_rad(float angle) { return angle * M_PI / 180.0; }

#pragma region random

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

#pragma endregion

#pragma region colors

inline sf::Color rainbow_function(const int _x) {
    int red, green, blue;
    double x = double(_x) / 1023 * 3 * M_PI / 2;
    if (x <= M_PI) red = int(sin(x + M_PI / 2) * 255);
    else red = int(sin(x + M_PI) * 255);
    green = int(sin(x) * 255);
    blue = int(sin(x + 3 * M_PI / 2) * 255);
    red = std::max(red, 0);
    green = std::max(green, 0);
    blue = std::max(blue, 0);
    return sf::Color(red, green, blue);
}

inline sf::Color get_color_from_image(const sf::Image& image, const sf::Vector2f position)
{
    return image.getPixel(static_cast<sf::Vector2u>(sf::Vector2f(position.x, position.y)));
}

inline sf::Color get_inverted_color(const sf::Color color)
{
    return sf::Color(255 - color.r, 255 - color.g, 255 - color.b, color.a);
}

#pragma endregion

#pragma region shapes

inline void set_pivot_center_to_shape(sf::Shape& shape)
{
    auto local = shape.getLocalBounds();
    shape.setOrigin({ local.position.x + local.size.x / 2.f, local.position.y + local.size.y / 2.f });
}

#pragma endregion

#pragma region  vectors

inline double euclidean_distance(const sf::Vector2f point1, const sf::Vector2f point2)
{
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    return std::sqrt(dx * dx + dy * dy);
}

inline sf::Vector2f get_vector_from_points(const sf::Vector2f point1, const sf::Vector2f point2)
{
    return sf::Vector2f(point2.x - point1.x, point2.y - point1.y);
}

inline sf::Vector2f get_vector_from_length_and_angle(float length, float angle_degrees)
{
    float angle_radians = angle_degrees * M_PI / 180.f;
    return sf::Vector2f(length * std::cos(angle_radians), length * std::sin(angle_radians));
}

inline double get_vector_length(sf::Vector2f vector) { return sqrt(pow(vector.x, 2) + pow(vector.y, 2)); }

inline sf::Vector2f get_vector_from_angle(float angle_degrees)
{
    float angle_radians = angle_degrees * M_PI / 180.f;
    return sf::Vector2f(std::cos(angle_radians), std::sin(angle_radians));
}

inline float angle_between_vectors_2d(const sf::Vector2f vector1, const sf::Vector2f vector2)
{
    float cross = vector1.x * vector2.y - vector1.y * vector2.x;
    float dot = vector1.x * vector2.x + vector1.y * vector2.y;
    return std::atan2(cross, dot);
}

inline sf::Vector2f get_normal_vector(sf::Vector2f vector)
{
    float length = get_vector_length(vector);
    if (length == 0) return { 0,0 };
    return { vector.x / length, vector.y / length };
}

#pragma endregion
#endif