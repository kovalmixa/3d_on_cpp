#ifndef VECTOR_FUNCTIONS_H
#define VECTOR_FUNCTIONS_H
#pragma once

#include <glm/glm.hpp>


inline float euclidean_distance(const glm::vec2& p1, const glm::vec2& p2) {
    return glm::distance(p1, p2);
}

inline glm::vec2 get_vector_from_length_and_angle(const float& length, const float& angle_degrees) {
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

inline glm::vec3 get_vec_mid(glm::vec3 p1, glm::vec3 p2) {
    return glm::vec3((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
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

//inline bool ray_aabb_intersection(const Ray& ray, const AABB& box) {
//    float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
//    float tmax = (box.max.x - ray.origin.x) / ray.direction.x;
//
//    if (tmin > tmax) std::swap(tmin, tmax);
//
//    float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
//    float tymax = (box.max.y - ray.origin.y) / ray.direction.y;
//
//    if (tymin > tymax) std::swap(tymin, tymax);
//
//    if ((tmin > tymax) || (tymin > tmax)) return false;
//
//    if (tymin > tmin) tmin = tymin;
//    if (tymax < tmax) tmax = tymax;
//
//    float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
//    float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;
//
//    if (tzmin > tzmax) std::swap(tzmin, tzmax);
//
//    if ((tmin > tzmax) || (tzmin > tmax)) return false;
//
//    return true;
//}

#endif