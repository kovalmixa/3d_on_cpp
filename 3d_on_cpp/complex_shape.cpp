#include "complex_shape.h"

void ComplexShape::apply_transform()
{
}

ComplexShape::ComplexShape()
{
}

ComplexShape::~ComplexShape()
{
}

void ComplexShape::set_transform(const sf::Vector2f position, const sf::Vector2f size, const sf::Angle rotation)
{
}

void ComplexShape::set_transform(const TransformData transform_data)
{
}

TransformData ComplexShape::get_transform()
{
	return TransformData();
}

sf::FloatRect ComplexShape::get_global_bounds()
{
	return sf::FloatRect();
}

void ComplexShape::set_outline(const bool show)
{

}

sf::Color ComplexShape::get_color()
{
	return sf::Color();
}

void ComplexShape::set_color(const sf::Color color)
{
}

void ComplexShape::draw(sf::RenderWindow& window)
{
}

bool ComplexShape::contains(const sf::Vector2f point)
{
	return false;
}

ComplexShape* ComplexShape::clone()
{
	return nullptr;
}
