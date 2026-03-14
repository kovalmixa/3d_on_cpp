#include <iostream>
#include <algorithm>
#include <SFML/System/Exception.hpp>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "ui_controller.h"
#include "logic_controller.h"
#include "selection_controller.h"

using json = nlohmann::json;

LogicController* LogicController::instance_ = nullptr;

LogicController::LogicController()
{
	handlers_ = {
		{ButtonAction::Paint,	 [this](sf::Vector2f position) { paint_figure(position); } },
		{ButtonAction::Pipette,  [this](sf::Vector2f position) { copy_color(position); }},
	};
}

LogicController::~LogicController()
{
	for (auto shape : shapes_) delete shape;
	shapes_.clear();
}

void LogicController::copy_color(sf::Vector2f position)
{
	auto ui_controller = UIController::get_instance();
	try
	{
		for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it) {
			if ((*it)->contains(position)) {
				(*it)->get_color();
				ui_controller->current_color = (*it)->get_color();
				break;
			}
		}
		ui_controller->current_button_action = ButtonAction::None;
	}
	catch (sf::Exception exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
	}
}

void LogicController::paint_figure(sf::Vector2f position)
{
	auto ui_controller = UIController::get_instance();
	try
	{
		for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it) {
			if ((*it)->contains(position)) {
				(*it)->set_color(ui_controller->current_color);
				break;
			}
		}
		ui_controller->background_color = ui_controller->current_color;
	}
	catch (sf::Exception exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
	}
}

void LogicController::try_find_shape_to_select(sf::Vector2f position)
{
	bool ctrl_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
	auto selection = SelectionController::get_instance();
	for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it)
		if (selection->try_select_shape(*it, position, ctrl_pressed)) return;
	selection->clear_selection();
}

LogicController* LogicController::get_instance() { return instance_ ? instance_ : instance_ = new LogicController(); }

void LogicController::load_data()
{
	json j;
}

void LogicController::save_data()
{
	json j;
}

void LogicController::execute_action(ButtonAction action, sf::Vector2f mouse_position)
{
	if (is_dragging_) return;
	auto it = handlers_.find(action);
	if (it != handlers_.end()) {
		SelectionController::get_instance()->clear_selection();
		it->second(mouse_position);
	}
	else try_find_shape_to_select(mouse_position);
}

void LogicController::keyboard_action_process(sf::Event event, sf::Vector2f mouse_position)
{
	if (!event.is<sf::Event::KeyPressed>()) return;
	const auto* key = event.getIf<sf::Event::KeyPressed>();
	auto selection_controller = SelectionController::get_instance();

	switch (key->code) {
	case sf::Keyboard::Key::Delete:
	case sf::Keyboard::Key::Backspace:
	{
		selection_controller->delete_selected_shapes();
		break;
	}
	case sf::Keyboard::Key::W:
	{
		selection_controller->transform_mode = TransformMode::Move;
		break;
	}
	case sf::Keyboard::Key::R:
	{
		selection_controller->transform_mode = TransformMode::Rotate;
		break;
	}
	}
	if (key->control)
	{
		switch (key->code)
		{
		case sf::Keyboard::Key::X:
		{
			selection_controller->delete_selected_shapes();
		}
		case sf::Keyboard::Key::C:
		{
			selection_controller->try_copy_shapes();
			break;
		}
		case sf::Keyboard::Key::V:
		{
			selection_controller->try_paste_shapes(mouse_position);
			break;
		}
		case sf::Keyboard::Key::A:
		{
			for (auto shape : shapes_) selection_controller->try_add_shape_to_selection(shape, true);
		}
		}
	}
}

void LogicController::begin_drag(sf::Vector2f mouse_position)
{
	if (is_dragging_) return;
	is_dragging_ = true;
	SelectionController::get_instance()->try_begin_drag_transform_mode(mouse_position);
}

void LogicController::end_drag()
{
	is_dragging_ = false;
	SelectionController::get_instance()->end_drag_transform_mode();
}

void LogicController::update_drag(ButtonAction action, sf::Vector2f mouse_position)
{
	if (!is_dragging_) return;
	auto selection = SelectionController::get_instance();
	if (action == ButtonAction::None) selection->update_transform(mouse_position);
}

void LogicController::remove_actions()
{
	UIController::get_instance()->current_button_action = ButtonAction::None;
	SelectionController::get_instance()->clear_selection();
}

void LogicController::add_shape(ComplexShape* shape) { shapes_.push_back(shape); }

void LogicController::delete_shape(ComplexShape*& shape)
{
	if (!shape) return;
	SelectionController::get_instance()->remove_shape_from_selection(shape);
	auto found = std::find(shapes_.begin(), shapes_.end(), shape);
	if (found == shapes_.end()) {
		shape = nullptr;
		return;
	}
	shapes_.erase(found);
	delete shape;
	shape = nullptr;
	end_drag();
}

void LogicController::render_shapes(sf::RenderWindow& window)
{
	for (auto& shape : shapes_) shape->draw(window);
	SelectionController::get_instance()->draw_selection(window);
}
