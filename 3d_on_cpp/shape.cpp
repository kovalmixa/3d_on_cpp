#include "shape.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>

void Shape::apply_transform()
{
	set_bounding_box();
}

void Shape::set_bounding_box()
{
    auto& verts = geometry_.vertices;
    if (verts.empty()) return;
    bounding_box_.min = verts[0];
    bounding_box_.max = verts[0];
    for (const auto& v : verts)
    {
        bounding_box_.min = glm::min(bounding_box_.min, v);
        bounding_box_.max = glm::max(bounding_box_.max, v);
    }
}

void Shape::draw_outline(const glm::mat4& projection, const glm::mat4& view, 
    const glm::mat4& model, bool is_perspective)
{
    if (!shader_status) return;

    shader_controller_->use();

    float scale_factor = 1.f;
    if (is_perspective) {
        glm::vec3 model_pos = glm::vec3(model[3]);
        glm::vec3 camera_pos = glm::vec3(glm::inverse(view)[3]);
        float distance = glm::length(model_pos - camera_pos);
        scale_factor = std::clamp(1.0f + distance * 0.02f, 1.02f, 2.f);
    }
    glm::mat4 outline_model = glm::scale(model, glm::vec3(scale_factor));

    shader_controller_->set_uniform(projection, view, outline_model);
    glUniform4f(shader_color_, 0.f, 0.f, 0.f, 1.f);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(geometry_.indices.size()),
        GL_UNSIGNED_INT,
        0
    );
    glBindVertexArray(0);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
}

void Shape::draw_shadow(ShaderController* shadow_shader, const glm::mat4& light_space_matrix)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform_.position);
    model = glm::rotate(model, glm::radians(transform_.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, (float)global_clock.getElapsedTime().asSeconds(), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform_.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, transform_.scale);

    shadow_shader->use();
    GLint modelLoc = glGetUniformLocation(shadow_shader->program, "u_model");
    GLint lightLoc = glGetUniformLocation(shadow_shader->program, "u_lightSpaceMatrix");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(lightLoc, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(geometry_.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Shape::Shape() : VAO(0), VBO(0), EBO(0) {
    shader_controller_ = ShaderController::get_instance();
    shader_status = shader_controller_->check_correctness();
    if (!shader_status) return;
    color_ = glm::vec4(1.f);
    transform_.position = glm::vec3(0.0f);
    transform_.rotation = glm::vec3(0.0f);
    transform_.scale = glm::vec3(1.0f);

    shader_color_ = glGetUniformLocation(shader_controller_->program, "u_color");
}

Shape::~Shape()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Shape::set_geometry(const Geometry& geometry) {
    geometry_ = geometry;

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        geometry_.vertices.size() * sizeof(glm::vec3),
        geometry_.vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        geometry_.indices.size() * sizeof(unsigned int),
        geometry_.indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    set_bounding_box();
}

void Shape::set_transform(const Transform& transform_data)
{
    transform_ = transform_data;
    apply_transform();
}

Transform Shape::get_transform() const { return transform_; }

void Shape::set_outline(const bool show)
{

}

glm::vec4 Shape::get_color()
{
  return color_;
}

void Shape::set_color(const glm::vec4& color)
{
}

AABB Shape::get_box() { return bounding_box_; }

void Shape::draw(sf::RenderWindow& window, glm::vec3 camera_pos, bool is_perspective)
{
    if (!shader_status) return;
    glEnable(GL_DEPTH_TEST);

    sf::Vector2u windowSize = window.getSize();
    glViewport(0, 0, windowSize.x, windowSize.y);

    float aspect = static_cast<float>(windowSize.x) /
        std::max(static_cast<float>(windowSize.y), 1.0f);
    glm::mat4 projection;
    if (is_perspective) projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    else {
        float size = 5.0f;
        projection = glm::ortho(-size * aspect, size * aspect, -size, size, 0.1f, 100.0f);
    }
    glm::mat4 view = glm::lookAt(camera_pos, camera_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform_.position);
    model = glm::rotate(model, glm::radians(transform_.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, (float)global_clock.getElapsedTime().asSeconds(), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform_.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, transform_.scale);

    draw_outline(projection, view, model, is_perspective);
    shader_controller_->use();
    shader_controller_->set_uniform(projection, view, model);

    glUniform4f(shader_color_, color_.r, color_.g, color_.b, color_.a);
    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(geometry_.indices.size()),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}

bool Shape::contains(const sf::Vector2f point)
{
	return false;
}

Shape* Shape::clone()
{
	return nullptr;
}