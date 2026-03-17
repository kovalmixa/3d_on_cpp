#include "transform_data.h"

const TransformData TransformData::Default{};

TransformData::TransformData() : position(0.f), size(0.f), rotation(0.f) {}

TransformData::TransformData(glm::vec3 position, glm::vec3 size, glm::vec3 rotation)
    : position(position), size(size), rotation(rotation) {
}