#pragma once
#include <deque>

#include "complex_shape.h"

struct save_data_container {
	std::deque<ComplexShape*> shapes_;
	sf::Color background_color;
	sf::Color current_color;
};