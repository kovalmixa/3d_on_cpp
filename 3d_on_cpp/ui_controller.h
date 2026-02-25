#pragma once
#include <map>
#include <string>

#include "imgui.h"

enum class ButtonAction
{
	None,
	Ellipse,
	Rectangle,
	Polygon,
	Pipette,
	Paint
};

class UIController
{
private:
	std::map<std::string, ButtonAction> buttons;
	static UIController* instance_;
	UIController() = default;
	static sf::Color current_color_;
public:
	static UIController* get_instance();
	ButtonAction current_action = ButtonAction::None;

	void button_click(std::string label, ButtonAction& tool);

	void set_current_color(const sf::Color& color);
	sf::Color get_current_color() const;

	void draw_button(const std::string& label, ButtonAction action, ImVec2 size = ImVec2(100, 20));
};