#pragma once
#include <glm/glm.hpp>

struct TransformData {
    static const TransformData Default;

    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;

    TransformData();
    TransformData(glm::vec3 position, glm::vec3 size, glm::vec3 rotation);
};