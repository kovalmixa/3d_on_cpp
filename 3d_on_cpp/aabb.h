#pragma once
#include <glm/glm.hpp>

struct AABB {
    static const AABB Default;
    glm::vec3 min;
    glm::vec3 max;
    AABB();
    AABB(glm::vec3 min, glm::vec3 max);

};