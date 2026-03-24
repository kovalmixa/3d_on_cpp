#pragma once
#include <deque>
#include <functional>
#include <map>
#include <glm/glm.hpp>

#include "shape.h"
#include "ui_controller.h"

class LogicController
{
private:
	static LogicController* instance_;
	const float MODELS_UPSCALE = 1.5;
	std::deque<Shape*> shapes_;
	std::map<ButtonAction, std::function<void(sf::Vector2f)>> handlers_;
	bool is_dragging_ = false;
	sf::Vector2f drag_start_;

	LogicController();
	~LogicController();

	void try_parse_obj_file(std::string file_path);

	void copy_color(sf::Vector2f position);
	void paint_shape(sf::Vector2f position);

	void try_find_shape_to_select(sf::Vector2f position);
public:
	static LogicController* get_instance();

	void load_shapes(std::string file_path);
	
	void execute_action(ButtonAction action, sf::Vector2f mouse_position);
	void keyboard_action_process(const sf::Event event, sf::Vector2f mouse_position);
	void process_camera_mov(float dt);
	void begin_drag(sf::Vector2f mouse_position);
	void end_drag();

	void update_drag(ButtonAction action, sf::Vector2f mouse_position);
	void remove_actions();
	void add_shape(Shape* shape);
	void delete_shape(Shape*& shape);
	void delete_all_shapes();
	void render_shapes(sf::RenderWindow& window);
};
