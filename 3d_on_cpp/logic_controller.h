#pragma once
#include "ui_controller.h"
#include <functional>
#include <map>

class LogicController {
private:
	static LogicController* instance_;

	std::map<ButtonAction, std::function<void(sf::Vector2f)>> handlers_;

	bool is_dragging_ = false;
	sf::Vector2f drag_start_;

	LogicController();
	~LogicController();

	void paint_figure(sf::Vector2f position);
	void spawn_rectangle(sf::Vector2f position);

	void try_find_shape_to_select(sf::Vector2f position);
public:
	static LogicController* get_instance();
	void execute_action(ButtonAction action, sf::Vector2f mouse_position);
	void begin_drag(sf::Vector2f mouse_position);
	void end_drag();
	void update_drag(ButtonAction action, sf::Vector2f mouse_position);
	void remove_actions();
	void render_shapes(sf::RenderWindow& window);
};