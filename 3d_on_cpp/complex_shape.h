#pragma once

#pragma once
#include <SFML/Graphics.hpp>

#include "transform_data.h"

class ComplexShape {
protected:
    sf::Shape* shape_ = nullptr;
    TransformData transform_data_;

    void apply_transform();
public:
    ComplexShape();
    ~ComplexShape();

    void set_transform(const sf::Vector2f position, const sf::Vector2f size, const sf::Angle rotation);
    void set_transform(const TransformData transform_data);
    TransformData get_transform();
    sf::FloatRect get_global_bounds();

    void set_outline(const bool show);
    sf::Color get_color();
    void set_color(const sf::Color color);

    void draw(sf::RenderWindow& window);

    bool contains(const sf::Vector2f point);

    ComplexShape* clone();
};
