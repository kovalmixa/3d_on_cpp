#include "transform_data.h"

const TransformData TransformData::Default{};

TransformData::TransformData() : position({ 0,0 }), size(0, 0), rotation(sf::degrees(0)) {}

TransformData::TransformData(sf::Vector2f position, sf::Vector2f size, sf::Angle rotation)
    : position(position), size(size), rotation(rotation) {
}