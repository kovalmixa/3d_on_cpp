#ifndef SFML_STATIC
#define SFML_STATIC
#endif

#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <string>
#include "math_functions.h"
#include "color_functions.h"
#include "file_functions.h"
#include "ui_controller.h"
#include "logic_controller.h"
#include "selection_controller.h"
#include "camera_controller.h"

using json = nlohmann::json;

LogicController* LogicController::instance_ = nullptr;

LogicController::LogicController()
{
	handlers_ = {
		{ButtonAction::Paint,	 [this](sf::Vector2f position) { paint_shape(position); } },
		{ButtonAction::Pipette,  [this](sf::Vector2f position) { copy_color(position); }},
	};
}

LogicController::~LogicController()
{
	for (auto shape : shapes_) delete shape;
	shapes_.clear();
}

void LogicController::try_parse_obj_file(std::string file_path)
{
	std::string text = read_file(file_path);
	if (text.empty()) return;
	delete_all_shapes();

	if (file_path.find(".obj") != std::string::npos) {
		std::vector<glm::vec3> all_file_verts;
		std::vector<unsigned int> current_object_inds;
		size_t vertices_before_object = 0;

		float max_v = std::numeric_limits<float>::lowest();
		float min_v = std::numeric_limits<float>::max();

		auto save_shape = [&]() {
			if (!current_object_inds.empty()) {
				std::vector<glm::vec3> shape_verts;
				for (size_t i = vertices_before_object; i < all_file_verts.size(); ++i) {
					glm::vec3 v = all_file_verts[i];
					v = glm::vec3(
						map_value<float>(v.x, min_v, max_v, -1, 1),
						map_value<float>(v.y, min_v, max_v, -1, 1),
						map_value<float>(v.z, min_v, max_v, -1, 1)
					);
					shape_verts.push_back(v);
				}
				Shape* new_shape = new Shape();
				new_shape->set_geometry(Geometry(shape_verts, current_object_inds));
				new_shape->set_color(glm::vec4(1, 1, 1, 1));
				shapes_.push_back(new_shape);

				vertices_before_object = all_file_verts.size();
				current_object_inds.clear();
				max_v = std::numeric_limits<float>::lowest();
				min_v = std::numeric_limits<float>::max();
			}
			};

		std::stringstream main_ss(text);
		std::string line;
		while (std::getline(main_ss, line)) {
			std::stringstream ss(line);
			std::string prefix;
			ss >> prefix;
			if (prefix == "o") save_shape();
			else if (prefix == "v") {
				glm::vec3 v;
				ss >> v.x >> v.y >> v.z;
				all_file_verts.push_back(v);
				max_v = std::max({ max_v, v.x, v.y, v.z });
				min_v = std::min({ min_v, v.x, v.y, v.z });
			}
			else if (prefix == "f") {
				std::string vertex_str;
				std::vector<unsigned int> face_indices;
				while (ss >> vertex_str) {
					size_t first_slash = vertex_str.find('/');
					int vIdx = std::stoi(vertex_str.substr(0, first_slash));

					unsigned int finalIdx = static_cast<unsigned int>(vIdx - 1 - vertices_before_object);
					face_indices.push_back(finalIdx);
				}
				for (size_t i = 1; i < face_indices.size() - 1; ++i) {
					current_object_inds.push_back(face_indices[0]);
					current_object_inds.push_back(face_indices[i]);
					current_object_inds.push_back(face_indices[i + 1]);
				}
			}
		}
		save_shape();
	}
}

void LogicController::copy_color(sf::Vector2f position)
{
	auto ui_controller = UIController::get_instance();
	try
	{
		for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it) {
			if ((*it)->contains(position)) {
				(*it)->get_color();
				ui_controller->current_color = glm_sf_col((*it)->get_color());
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

void LogicController::paint_shape(sf::Vector2f position)
{
	auto ui_controller = UIController::get_instance();
	try
	{
		for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it) {
			if ((*it)->contains(position)) {
				(*it)->set_color(sf_glm_col(ui_controller->current_color));
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

void LogicController::load_shapes(std::string file_path)
{
	delete_all_shapes();
	if (file_path.find(".obj") != std::string::npos) {
		try_parse_obj_file(file_path);
		return;
	}
	try {
		std::string text = read_file(file_path);
		if (text.empty()) return;
		json data = json::parse(text);
		if (data.contains("shapes") && data["shapes"].is_array()) {
			for (const auto& item : data["shapes"]) {
				Shape* new_shape = new Shape();
				item.get_to(*new_shape);
				shapes_.push_back(new_shape);
			}
		}
		std::cout << "successfully loadded json" << std::endl;
	}
	catch (json::exception& e) {
		std::cerr << "JSON Load Error: " << e.what() << std::endl;
	}
}

#pragma region execution methods

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
	if (selection_controller->is_selection_active) {
		switch (key->code) {
		case sf::Keyboard::Key::Delete:
		case sf::Keyboard::Key::Backspace: { selection_controller->delete_selected_shapes(); break; }
		case sf::Keyboard::Key::W: { selection_controller->transform_mode = TransformMode::Move; break; }
		case sf::Keyboard::Key::R: { selection_controller->transform_mode = TransformMode::Rotate; break; }
		}
		if (key->control)
		{
			switch (key->code)
			{
			case sf::Keyboard::Key::X: { selection_controller->delete_selected_shapes(); }
			case sf::Keyboard::Key::C: { selection_controller->try_copy_shapes(); break; }
			}
		}
	}
	else {
		switch (key->code) {
			CameraController::get_instance()->process_keyboard(key);
			if (key->control)
			{
				switch (key->code)
				{
				case sf::Keyboard::Key::A: {
					for (auto shape : shapes_) selection_controller->try_add_shape_to_selection(shape, true);
				}
				}
			}
		}
	}
	switch (key->code) {
		if (key->control)
		{
			switch (key->code) {
			case sf::Keyboard::Key::V: { selection_controller->try_paste_shapes(mouse_position); break; }
			}
		}
	}
}

#pragma endregion

#pragma region drag methods

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

#pragma endregion

void LogicController::remove_actions()
{
	UIController::get_instance()->current_button_action = ButtonAction::None;
	SelectionController::get_instance()->clear_selection();
}

void LogicController::add_shape(Shape* shape) { shapes_.push_back(shape); }

void LogicController::delete_shape(Shape*& shape)
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

void LogicController::delete_all_shapes()
{
	for (auto& shape : shapes_) delete_shape(shape);
}

void LogicController::render_shapes(sf::RenderWindow& window)
{
	for (auto& shape : shapes_) 
		shape->draw(window, CameraController::get_instance()->position);
	SelectionController::get_instance()->draw_selection(window);
}