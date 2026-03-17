#pragma once
#include <deque>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>

#include "transform_data.h"

class ComplexShape {
protected:
    std::deque<ComplexShape*> shapes_;
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
    glm::vec4 get_color();
    void set_color(const glm::vec4 color);

    void draw(sf::RenderWindow& window);

    bool contains(const sf::Vector2f point);

    ComplexShape* clone();
};
