#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <random>

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
#pragma endregion

#pragma region vectors
inline float euclidean_distance(const glm::vec2& p1, const glm::vec2& p2) {
    return glm::distance(p1, p2);
}

inline glm::vec2 get_vector_from_length_and_angle(float length, float angle_degrees) {
    float rad = glm::radians(angle_degrees);
    return glm::vec2(length * std::cos(rad), length * std::sin(rad));
}

inline float get_vector_length(const glm::vec2& v) { return glm::length(v); }

inline float angle_between_vectors_2d(const glm::vec2& v1, const glm::vec2& v2) {
    return std::atan2(v1.x * v2.y - v1.y * v2.x, glm::dot(v1, v2));
}

inline glm::vec2 get_normal_vector(const glm::vec2& v) {
    if (glm::length(v) < 0.0001f) return glm::vec2(0.0f);
    return glm::normalize(v);
}

inline std::optional<float> ray_triangle_intersect(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2)
{
    const float EPSILON = 1e-8;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDirection, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return std::nullopt;

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) return std::nullopt;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDirection, q);

    if (v < 0.0f || u + v > 1.0f) return std::nullopt;

    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) return t;
    else return std::nullopt;
}

#pragma endregion

#endif