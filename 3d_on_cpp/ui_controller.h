#pragma once
#include <map>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <algorithm>

#include "imgui.h"

enum class ButtonAction
{
	None,
	Pipette,
	Paint,
	Perspective
};

class UIController
{
private:
	std::map<std::string, ButtonAction> buttons_;
	std::map<std::string, bool> checks_states_;

	static UIController* instance_;
	const ImVec4 ACTIVE_BTN_COLOR = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);

	UIController() = default;
	~UIController() = default;

	void show_activeness(const bool is_active);
	void button_click(std::string label, ButtonAction& tool);
public:
	ButtonAction current_button_action = ButtonAction::None;
	std::vector<ButtonAction> check_box_actions;
	sf::Color background_color = sf::Color(30, 30, 30);
	sf::Color current_color = sf::Color::White;

	static UIController* get_instance();

	bool is_action_active(const ButtonAction& action);
	void draw_checkbox(const std::string& label, ButtonAction action);
	void draw_once_act_button(std::string& label, void (*func)());
	void draw_act_button(const std::string& label, ButtonAction action);
};