#pragma once
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

#include "imgui.h"

enum class ButtonAction
{
	None,
	Pipette,
	Paint,
};

class UIController
{
private:
	std::map<std::string, ButtonAction> buttons;
	static UIController* instance_;
	UIController();

public:
	ButtonAction current_button_action = ButtonAction::None;
	sf::Color background_color = sf::Color(30, 30, 30);
	sf::Color current_color = sf::Color::White;

	static UIController* get_instance();

	void button_click(std::string label, ButtonAction& tool);
	void draw_canvas_act_button(const std::string& label, ButtonAction action, ImVec2 size = ImVec2(100, 20));
};