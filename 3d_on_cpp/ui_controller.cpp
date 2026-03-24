#include <imgui-sfml.h>
#include <imgui.h>

#include "ui_controller.h"

UIController* UIController::instance_ = nullptr;

void UIController::show_activeness(bool is_active)
{
    if (!is_active) return;
    ImGui::PushStyleColor(ImGuiCol_Button, ACTIVE_BTN_COLOR);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

UIController* UIController::get_instance() { return instance_ ? instance_ : (instance_ = new UIController()); }

void UIController::button_click(std::string label, ButtonAction& selected_action)
{
    auto it = buttons_.find(label);
    if (it == buttons_.end()) return;
    if (current_button_action == ButtonAction::None || current_button_action != selected_action)
        current_button_action = it->second;
    else if (current_button_action == selected_action) 
        current_button_action = ButtonAction::None;
}

bool UIController::is_action_active(const ButtonAction& action)
{
    return (current_button_action == action || std::find(check_box_actions.begin(),
        check_box_actions.end(), action) != check_box_actions.end()) && action != ButtonAction::None;
}

void UIController::draw_checkbox(const std::string& label, ButtonAction action)
{
    if (checks_states_.find(label) == checks_states_.end()) checks_states_[label] = false;

    if (ImGui::Checkbox(label.c_str(), &checks_states_[label]))
    {
        if (checks_states_[label] && !is_action_active(action)) check_box_actions.push_back(action);
        else check_box_actions.erase(std::remove(check_box_actions.begin(), 
            check_box_actions.end(), action), check_box_actions.end());
    }
}

void UIController::draw_once_act_button(std::string& label, void(*func)())
{
    if (ImGui::Button(label.c_str(), ImVec2(label.size() * 10, 20))) func();
}

void UIController::draw_act_button(const std::string& label, ButtonAction action = ButtonAction::None)
{
    if (buttons_.find(label) == buttons_.end() && action != ButtonAction::None)
        buttons_.insert(std::pair<const std::string, ButtonAction>(label, action));
    bool is_active = is_action_active(action);
    if (is_active) {
        ImGui::PushStyleColor(ImGuiCol_Button, ACTIVE_BTN_COLOR);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ACTIVE_BTN_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    }

    if (ImGui::Button(label.c_str(), ImVec2(label.size() * 10, 20))) button_click(label, action);

    if (is_active) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
}