#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>
#include <vector>

#include "basic_functions.h"
#include "logic_controller.h"
#include "ui_controller.h"

static std::atomic running(true);
static std::mutex event_mutex;
static std::vector<sf::Event> event_queue;
static sf::Clock delta_clock;
static int x = 0;

void logic_pipeline(sf::RenderWindow* window, const std::vector<sf::Event>& events)
{
    auto logic_controller = LogicController::get_instance();
    auto ui_controller = UIController::get_instance();
    for (auto& event : events)
    {
        ImGui::SFML::ProcessEvent(*window, event);

        // Mouse pressed
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
        {
            bool overUI = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
            if (!overUI)
            {
                if (mouseEvent->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f mouse_position = window->mapPixelToCoords(mouseEvent->position);
                    logic_controller->execute_action(ui_controller->current_button_action, mouse_position);
                    logic_controller->begin_drag(mouse_position);
                }
                else if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
                {
                    if (mouseEvent->button == sf::Mouse::Button::Left)
                        logic_controller->end_drag();
                }
                else if (mouseEvent->button == sf::Mouse::Button::Right)
                {

                }
            }
            else if (mouseEvent->button == sf::Mouse::Button::Right) logic_controller->remove_actions();
            if (mouseEvent->button == sf::Mouse::Button::Left)
            {
                // Handle UI interactions if necessary
            }
        }

        //Mouse released
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
            if (mouseEvent->button == sf::Mouse::Button::Left)
                logic_controller->end_drag();

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

    // Mouse drag handling
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldPos = window->mapPixelToCoords(pixelPos);
        logic_controller->update_drag(ui_controller->current_button_action, worldPos);
    }

    event_queue.clear();
}

void rendering_pipeline(sf::RenderWindow* window, ImGuiIO* io) {
    ImGui::SFML::Update(*window, delta_clock.restart());
    auto ui = UIController::get_instance();
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::BeginGroup();
#pragma region color_pallete_picker_code
        sf::Color current_color = ui->current_color;
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
            ui->current_color = sf::Color(
                static_cast<uint8_t>(color_array[0] * 255),
                static_cast<uint8_t>(color_array[1] * 255),
                static_cast<uint8_t>(color_array[2] * 255),
                static_cast<uint8_t>(color_array[3] * 255)
            );
        }
#pragma endregion
        ImGui::Text("Color picker:");
        ui->draw_button("Paint", ButtonAction::Paint, ImVec2(50, 20));
        ui->draw_button("Pipette", ButtonAction::Pipette);
        ImGui::EndGroup();
        ImGui::EndMainMenuBar();
    }

    window->clear(ui->background_color);
    //window->clear(rainbow_function(x));

    LogicController::get_instance()->render_shapes(*window);
    ImGui::SFML::Render(*window);

    window->display();
}

void window_render_thread(sf::RenderWindow* window, ImGuiIO* io)
{
    window->setActive(true);
    while (running)
    {
        x = x >= 1023 ? 0 : ++x;
        std::vector<sf::Event> current_frame_events;
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            current_frame_events.swap(event_queue);
        }
        logic_pipeline(window, current_frame_events);
        rendering_pipeline(window, io);
    }
    ImGui::SFML::Shutdown();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML 3d");
    window.setActive(false);
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();

    std::thread render(window_render_thread, &window, &io);

    while (running)
    {
        while (const std::optional event = window.pollEvent())
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            event_queue.push_back(*event);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (render.joinable()) render.join();
    return 0;
}

//int main() {
//    // 1. Initialize SFML Window with OpenGL
//    sf::Window window(sf::VideoMode({ 800, 600 }), "SFML 3D Cube", sf::Style::Default, sf::ContextSettings(32));
//    window.setVerticalSyncEnabled(true);
//    window.setActive(true);
//
//    // 2. Setup OpenGL Perspective
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.f, 800.f / 600.f, 1.f, 500.f);
//    glEnable(GL_DEPTH_TEST);
//
//    sf::Clock clock;
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) window.close();
//        }
//
//        // 3. Render 3D Scene
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        glTranslatef(0.f, 0.f, -200.f);
//        glRotatef(clock.getElapsedTime().asSeconds() * 50.f, 1.f, 1.f, 0.f);
//
//        // Draw cube (simplified faces)
//        glBegin(GL_QUADS);
//        glColor3f(1.f, 0.f, 0.f); glVertex3f(-50, -50, -50); glVertex3f(-50, 50, -50);
//        glVertex3f(50, 50, -50); glVertex3f(50, -50, -50);
//        // ... (Add other 5 faces here) ...
//        glEnd();
//
//        window.display();
//    }
//    return 0;
//}