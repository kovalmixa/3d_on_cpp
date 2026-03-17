#include "aabb.h"

const AABB AABB::Default{};

AABB::AABB() {
    min = glm::vec3(0.f);
    max = glm::vec3(0.f);
}

AABB::AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}