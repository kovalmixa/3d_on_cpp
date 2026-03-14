#pragma once
#include <SFML/Graphics.hpp>

struct TransformData {
    static const TransformData Default;

    sf::Vector2f position;
    sf::Vector2f size;
    sf::Angle rotation;

    TransformData();
    TransformData(sf::Vector2f position, sf::Vector2f size, sf::Angle rotation);
};