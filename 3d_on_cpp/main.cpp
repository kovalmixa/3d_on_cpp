#ifndef SFML_STATIC
#define SFML_STATIC
#endif

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include <imgui.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>
#include <vector>

#include <GL/glew.h>

#include "logic_controller.h"
#include "selection_controller.h"
#include "ui_controller.h"

static std::atomic running(true);
static std::mutex event_mutex;
static std::vector<sf::Event> event_queue;
static sf::Clock delta_clock;
std::atomic<bool> is_paused{ false };

void logic_pipeline(sf::RenderWindow* window, const std::vector<sf::Event>& events, float dt)
{
    auto logic_controller = LogicController::get_instance();
    auto ui_controller = UIController::get_instance();
    sf::Vector2f mouse_position;

    for (auto& event : events)
    {
        ImGui::SFML::ProcessEvent(*window, event);
        mouse_position = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

        // Mouse pressed
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
        {
            sf::Vector2f mouse_position = window->mapPixelToCoords(mouseEvent->position);
            bool overUI = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
            if (!overUI)
            {
                if (mouseEvent->button == sf::Mouse::Button::Left)
                {
                    if (ui_controller->current_button_action == ButtonAction::None &&
                        SelectionController::get_instance()->is_point_on_selection(mouse_position))
                    {
                        logic_controller->begin_drag(mouse_position);
                    }
                    else logic_controller->execute_action(ui_controller->current_button_action, mouse_position);
                }
                else if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
                    if (mouseEvent->button == sf::Mouse::Button::Left) logic_controller->end_drag();
                    else if (mouseEvent->button == sf::Mouse::Button::Right &&
                        SelectionController::get_instance()->is_point_on_selection(mouse_position))
                    {

                    }
            }
            if (mouseEvent->button == sf::Mouse::Button::Right) logic_controller->remove_actions();
        }

        //Mouse released
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
            if (mouseEvent->button == sf::Mouse::Button::Left)
                logic_controller->end_drag();

        logic_controller->keyboard_action_process(event, mouse_position);

        if (event.is<sf::Event::Closed>())
        {
            running = false;
            window->close();
        }
        if (const auto* resized = event.getIf<sf::Event::Resized>()) {
            sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
            window->setView(sf::View(visibleArea));
        }
    }

    logic_controller->process_camera_mov(dt);

    // Mouse drag handling
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldPos = window->mapPixelToCoords(pixelPos);
        logic_controller->update_drag(ui_controller->current_button_action, worldPos);
    }

}

void rendering_pipeline(sf::RenderWindow* window) {
    ImGui::SFML::Update(*window, delta_clock.restart());
    auto ui = UIController::get_instance();
    auto lc = LogicController::get_instance();
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::BeginGroup();
#pragma region color_pallete_picker_code
        auto current_color = ui->current_color;
        float color_array[4];
        color_array[0] = (float)current_color.r / 255.0f;
        color_array[1] = (float)current_color.g / 255.0f;
        color_array[2] = (float)current_color.b / 255.0f;
        color_array[3] = (float)current_color.a / 255.0f;

        if (ImGui::ColorEdit4("##ColorPicker", color_array,
            ImGuiColorEditFlags_PickerHueWheel |
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoLabel)
            )
        {
            ui->current_color = (sf::Color(
                static_cast<uint8_t>(color_array[0] * 255),
                static_cast<uint8_t>(color_array[1] * 255),
                static_cast<uint8_t>(color_array[2] * 255),
                static_cast<uint8_t>(color_array[3] * 255)
            ));
        }
#pragma endregion
        ImGui::Text("Color picker:");
        ui->draw_act_button("Paint", ButtonAction::Paint);
        ui->draw_act_button("Pipette", ButtonAction::Pipette);
        ui->draw_checkbox("Persp", ButtonAction::Perspective);
        ImGui::EndGroup();

        ImGui::BeginGroup();
        //if (ImGui::Button("Save", ImVec2(50, 20))) {
        //    IGFD::FileDialogConfig config;
        //    config.path = ".";
        //    ImGuiFileDialog::Instance()->OpenDialog("ChooseDirSave", "Choose Directory", ".json, .txt", config);
        //}
        if (ImGui::Button("Load", ImVec2(50, 20))) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDirLoad", "Choose File", ".obj, .json, .txt", config);
        }
        ImGui::EndGroup();

        ImGui::EndMainMenuBar();
    }
#pragma region dialog windows
    if (ImGuiFileDialog::Instance()->Display("ChooseDirLoad")) {
        if (ImGuiFileDialog::Instance()->IsOk())
            lc->load_shapes(ImGuiFileDialog::Instance()->GetFilePathName());
        ImGuiFileDialog::Instance()->Close();
    }
#pragma endregion
    window->clear(ui->background_color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    LogicController::get_instance()->render_shapes(*window);
    ImGui::SFML::Render(*window);

    window->display();
}

void window_render_thread(sf::RenderWindow* window)
{
    window->setActive(true);
    glewInit();

    ImGui::SFML::Init(*window);

    while (running)
    {
        if (is_paused) continue;
        float dt = delta_clock.restart().asSeconds();
       
        std::vector<sf::Event> current_frame_events;
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            current_frame_events.swap(event_queue);
            event_queue.clear();
        }

        logic_pipeline(window, current_frame_events, dt);
        rendering_pipeline(window);
    }
    ImGui::SFML::Shutdown();
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML 3d");
    
    window.setFramerateLimit(60);
    window.setActive(false);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    std::thread render(window_render_thread, &window);

    while (running)
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) running = false;
            
            if (event->is<sf::Event::FocusLost>()) is_paused = true;
            else if (event->is<sf::Event::FocusGained>()) is_paused = false;

            std::lock_guard<std::mutex> lock(event_mutex);
            event_queue.push_back(*event);
        }
    }

    if (render.joinable()) render.join();
    return 0;
}