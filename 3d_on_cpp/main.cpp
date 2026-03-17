#include <imgui-SFML.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "basic_functions.h"
#include "logic_controller.h"
#include "ui_controller.h"
#include "basic_functions.h"

static std::atomic running(true);
static std::mutex event_mutex;
static std::vector<sf::Event> event_queue;
static sf::Clock delta_clock;
static sf::Clock global_clock; // Не перезапускать!
GLuint globalVAO, globalVBO, globalEBO;

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

void draw_3d_scene(sf::RenderWindow& window) {
    window.pushGLStates();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Настройка камеры
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.f, 800.f / 600.f, 0.1f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -5.f);
    glRotatef((float)global_clock.getElapsedTime().asSeconds() * 50.f, 0.5f, 1.f, 0.f);

    // Чтобы куб не был одноцветным пятном, можно включить режим линий
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0f, 1.0f, 1.0f); // Белый цвет линий

    glBindVertexArray(globalVAO);
    // 36 — это количество индексов в массиве indices
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Возвращаем заливку для остального
    glDisable(GL_DEPTH_TEST);
    window.popGLStates();
}

void init_opengl_resources() {
    glewInit();

    // 8 уникальных вершин куба (X, Y, Z)
    float vertices[] = {
        -0.5f, -0.5f,  0.5f, // 0: перед-низ-лево
         0.5f, -0.5f,  0.5f, // 1: перед-низ-право
         0.5f,  0.5f,  0.5f, // 2: перед-верх-право
        -0.5f,  0.5f,  0.5f, // 3: перед-верх-лево
        -0.5f, -0.5f, -0.5f, // 4: зад-низ-лево
         0.5f, -0.5f, -0.5f, // 5: зад-низ-право
         0.5f,  0.5f, -0.5f, // 6: зад-верх-право
        -0.5f,  0.5f, -0.5f  // 7: зад-верх-лево
    };

    // Порядок отрисовки треугольников (12 штук)
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0, // Перед
        1, 5, 6,  6, 2, 1, // Право
        7, 6, 5,  5, 4, 7, // Зад
        4, 0, 3,  3, 7, 4, // Лево
        4, 5, 1,  1, 0, 4, // Низ
        3, 2, 6,  6, 7, 3  // Верх
    };

    glGenVertexArrays(1, &globalVAO);
    glGenBuffers(1, &globalVBO);
    glGenBuffers(1, &globalEBO);

    glBindVertexArray(globalVAO);

    // Вершины
    glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Атрибут позиции (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void rendering_pipeline(sf::RenderWindow* window) {
    ImGui::SFML::Update(*window, delta_clock.restart());
    auto ui = UIController::get_instance();
    auto lc = LogicController::get_instance();
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::BeginGroup();
#pragma region color_pallete_picker_code
        float color_array[4];
        color_array[0] = ui->current_color.r;
        color_array[1] = ui->current_color.g;
        color_array[2] = ui->current_color.b;
        color_array[3] = ui->current_color.a;

        if (ImGui::ColorEdit4("##ColorPicker", color_array,
            ImGuiColorEditFlags_PickerHueWheel |
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoLabel)
            )
        {
            ui->current_color = glm::vec4(
                static_cast<uint8_t>(color_array[0]),
                static_cast<uint8_t>(color_array[1]),
                static_cast<uint8_t>(color_array[2]),
                static_cast<uint8_t>(color_array[3])
            );
        }
#pragma endregion
        ImGui::Text("Color picker:");
        ui->draw_canvas_act_button("Paint", ButtonAction::Paint, ImVec2(50, 20));
        ui->draw_canvas_act_button("Pipette", ButtonAction::Pipette);
        ImGui::EndGroup();

        ImGui::BeginGroup();
        if (ImGui::Button("Save", ImVec2(50, 20))) lc->save_data();
        if (ImGui::Button("Load", ImVec2(50, 20))) lc->load_data();
        ImGui::EndGroup();

        ImGui::EndMainMenuBar();
    }
    window->clear(sf::Color(
        static_cast<uint8_t>(ui->background_color.r * 255),
        static_cast<uint8_t>(ui->background_color.g * 255),
        static_cast<uint8_t>(ui->background_color.b * 255),
        static_cast<uint8_t>(ui->background_color.a * 255)
    ));
    //window->clear(rainbow_function(x));

    draw_3d_scene(*window);
    LogicController::get_instance()->render_shapes(*window);
    ImGui::SFML::Render(*window);

    window->display();
}

void window_render_thread(sf::RenderWindow* window)
{
    window->setActive(true);
    init_opengl_resources();
    while (running)
    {
        x = x >= 1023 ? 0 : ++x;
        std::vector<sf::Event> current_frame_events;
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            current_frame_events.swap(event_queue);
        }
        logic_pipeline(window, current_frame_events);
        rendering_pipeline(window);
    }
    ImGui::SFML::Shutdown();
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML 3d");
    window.setActive(false);
    window.setFramerateLimit(60);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ImGui::SFML::Init(window);

    std::thread render(window_render_thread, &window);

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