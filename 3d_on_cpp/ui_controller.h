#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>

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
	glm::vec4 background_color = glm::vec4(0.8f);
	glm::vec4 current_color = glm::vec4(1.0f);

	static UIController* get_instance();

	void button_click(std::string label, ButtonAction& tool);
	void draw_canvas_act_button(const std::string& label, ButtonAction action, ImVec2 size = ImVec2(100, 20));
};