#include <iostream>
#include <algorithm>
#include <SFML/System/Exception.hpp>
#include <SFML/Graphics.hpp>

#include "ui_controller.h"
#include "logic_controller.h"

LogicController* LogicController::instance_ = nullptr;

LogicController::LogicController()
{

}

LogicController::~LogicController()
{

}

void LogicController::paint_figure(sf::Vector2f position)
{
	try
	{
	}
	catch (sf::Exception exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
	}
}

void LogicController::spawn_rectangle(sf::Vector2f position)
{
	try
	{

	}
	catch (sf::Exception exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
	}
}

void LogicController::try_find_shape_to_select(sf::Vector2f position)
{

}

LogicController* LogicController::get_instance() { return instance_ ? instance_ : instance_ = new LogicController(); }

void LogicController::execute_action(ButtonAction action, sf::Vector2f mouse_position)
{
	auto it = handlers_.find(action);
	if (it != handlers_.end()) it->second(mouse_position);
	else try_find_shape_to_select(mouse_position);
}

void LogicController::begin_drag(sf::Vector2f mouse_position)
{
	is_dragging_ = true;
	//SelectionController::get_instance()->begin_drag_transform_mode(mouse_position);
}

void LogicController::end_drag()
{
	is_dragging_ = false;
	//SelectionController::get_instance()->end_drag_transform_mode();
}

void LogicController::update_drag(ButtonAction action, sf::Vector2f mouse_position)
{
	if (!is_dragging_) return;
}

void LogicController::remove_actions()
{

}

void LogicController::render_shapes(sf::RenderWindow& window)
{

}
