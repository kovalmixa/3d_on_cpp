#include <imgui-sfml.h>
#include <imgui.h>

#include "ui_controller.h"

UIController* UIController::instance_ = nullptr;
sf::Color UIController::current_color = sf::Color::White;
sf::Color UIController::background_color = sf::Color(170, 170, 170);

UIController::UIController() {
}

UIController* UIController::get_instance() { return instance_ ? instance_ : (instance_ = new UIController()); }

void UIController::button_click(std::string label, ButtonAction& selected_action)
{
    auto it = buttons.find(label);
    if (it == buttons.end()) return;
    if (current_button_action == ButtonAction::None || current_button_action != selected_action)
    {
        current_button_action = it->second;
        selected_action = current_button_action;
    }
    else if (current_button_action == selected_action) {
        current_button_action = ButtonAction::None;
        selected_action = current_button_action;
    }
}

void UIController::draw_button(const std::string& label, ButtonAction action = ButtonAction::None, ImVec2 size)
{
    auto it = buttons.find(label);
    if (it == buttons.end()) buttons.insert(std::pair<const std::string, ButtonAction>(label, action));
    bool active = (action == current_button_action && action != ButtonAction::None);

    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    }
    if (ImGui::Button(label.c_str(), size)) button_click(label, action);
    if (active)
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}