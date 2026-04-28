#pragma once
#include <deque>
#include <functional>
#include <map>
#include <glm/glm.hpp>

#include "shape.h"
#include "ui_controller.h"
#include "ray.h"

class LogicController
{
private:
	static LogicController* instance_;
	const float MODELS_UPSCALE = 1.5;
	const float SHAPES_MOVE_SPEED = 0.1;
	glm::vec3 target_pos;
	glm::vec3 current_ray_dir;
	bool has_target = false;
	std::deque<Shape*> shapes_;
	std::map<ButtonAction, std::function<void(const Ray&)>> handlers_;
	bool is_dragging_ = false;
	sf::Vector2f drag_start_;
	LogicController();
	~LogicController();

	void try_parse_obj_file(std::string file_path);

	void copy_color(const Ray& ray);
	void paint_shape(const Ray& ray);
	void try_find_shape_to_select(const Ray& ray);
	ViewProjection* get_vp(sf::RenderWindow& window);
public:
	static LogicController* get_instance();

	void load_shapes(std::string file_path);
	
	void execute_action(sf::RenderWindow& window, ButtonAction action, sf::Vector2f mouse_position);
	void keyboard_action_process(const sf::Event event, sf::Vector2f mouse_position);
	void process_keyboard_delta_mov(float dt);
	void set_global_transform_bias(const Transform& transform_data);
	void set_trajectory(sf::RenderWindow& window, sf::Vector2f mouse_position);
	void begin_drag(sf::Vector2f mouse_position);
	void end_drag();

	void update_drag(ButtonAction action, sf::Vector2f mouse_position);
	void remove_actions();
	void add_shape(Shape* shape);
	void delete_shape(Shape*& shape);
	void delete_all_shapes();
	void render_shapes(sf::RenderWindow& window);
};
