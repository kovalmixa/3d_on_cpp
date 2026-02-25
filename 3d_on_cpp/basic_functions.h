#pragma once
#include <algorithm>
#include <corecrt_math_defines.h>
#include <SFML/Graphics.hpp>

static sf::Color rainbow_function(const int _x) {
    int red, green, blue;
    double x = double(_x) / 1023 * 3 * M_PI / 2;
    if (x <= M_PI) red = int(sin(x + M_PI / 2) * 255);
    else red = int(sin(x + M_PI) * 255);
    green = int(sin(x) * 255);
    blue = int(sin(x + 3 * M_PI / 2) * 255);
    red = std::max(red, 0);
    green = std::max(green, 0);
    blue = std::max(blue, 0);
    return sf::Color(red, green, blue);
}

static sf::Color get_color_from_image(const sf::Image& image, const sf::Vector2f position)
{
    return image.getPixel(static_cast<sf::Vector2u>(sf::Vector2f(position.x, position.y)));
}

static sf::Color get_inverted_color(const sf::Color color)
{
    return sf::Color(255 - color.r, 255 - color.g, 255 - color.b, color.a);
}

static void set_pivot_center_to_shape(sf::Shape& shape)
{
    auto local = shape.getLocalBounds();
    shape.setOrigin({ local.size.x / 2.f, local.size.y / 2.f });
}

static double euclidean_distance(const sf::Vector2f point1, const sf::Vector2f point2) {
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    return std::sqrt(dx * dx + dy * dy);
}

static sf::Vector2f get_vector_from_points(const sf::Vector2f point1, const sf::Vector2f point2)
{
    return sf::Vector2f(point2.x - point1.x, point2.y - point1.y);
}

static sf::Vector2f get_vector_from_length_and_angle(float length, float angle_degrees) {
    float angle_radians = angle_degrees * M_PI / 180.f;
    return sf::Vector2f(length * std::cos(angle_radians), length * std::sin(angle_radians));
}

static sf::Vector2f get_vector_from_angle(float angle_degrees) {
    float angle_radians = angle_degrees * M_PI / 180.f;
    return sf::Vector2f(std::cos(angle_radians), std::sin(angle_radians));
}

static double angle_between_vectors_2d(const sf::Vector2f vector1, const sf::Vector2f vector2) {
    double cross = vector1.x * vector2.y - vector1.y * vector2.x;
    double dot = vector1.x * vector2.x + vector1.y * vector2.y;
    return std::atan2(cross, dot);
}
