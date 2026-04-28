#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm {
    inline void to_json(nlohmann::json& j, const vec3& v) {
        j = nlohmann::json{ v.x, v.y, v.z };
    }

    inline void from_json(const nlohmann::json& j, vec3& v) {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    inline void to_json(nlohmann::json& j, const vec4& v) {
        j = nlohmann::json{ v.r, v.g, v.b, v.a };
    }

    inline void from_json(const nlohmann::json& j, vec4& v) {
        v.r = j.at(0).get<float>();
        v.g = j.at(1).get<float>();
        v.b = j.at(2).get<float>();
        v.a = j.at(3).get<float>();
    }
}

struct Transform {
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
};

inline Transform operator + (const Transform& lhs, const Transform& rhs) {
    Transform result;
    result.position = lhs.position + rhs.position;
    result.rotation = lhs.rotation + rhs.rotation;
    result.scale = lhs.scale * rhs.scale;
    return result;
}

inline Transform operator - (const Transform& lhs, const Transform& rhs) {
    Transform result;
    result.position = lhs.position - rhs.position;
    result.rotation = lhs.rotation - rhs.rotation;
    result.scale = lhs.scale / rhs.scale;
    return result;
}

inline void to_json(nlohmann::json& j, const Transform& t) {
    j = nlohmann::json{ {"position", t.position}, {"rotation", t.rotation}, {"scale", t.scale} };
}

inline void from_json(const nlohmann::json& j, Transform& t) {
    j.at("position").get_to(t.position);
    j.at("rotation").get_to(t.rotation);
    j.at("scale").get_to(t.scale);
}