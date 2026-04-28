#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shape.h"
#include "vector_functions.h"

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

glm::mat4 Shape::get_model()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform_.position);
    model = glm::rotate(model, glm::radians(transform_.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(transform_.rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform_.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, transform_.scale);
    return model;
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

AABB Shape::get_box(std::list<glm::vec3>& vertices)
{

    return AABB();
}

void Shape::draw(sf::RenderWindow& window, ViewProjection& vp)
{
    if (!shader_status) return;
    glEnable(GL_DEPTH_TEST);

    sf::Vector2u windowSize = window.getSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glm::mat4 model = get_model();

    draw_outline(vp.projection, vp.view, model, vp.is_perspective);
    shader_controller_->use();
    shader_controller_->set_uniform(vp.projection, vp.view, model);

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

bool Shape::contains(const Ray& ray, bool is_box)
{
    glm::mat4 invModel = glm::inverse(get_model());

    Ray localRay;
    localRay.origin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
    localRay.direction = glm::normalize(glm::vec3(invModel * glm::vec4(ray.direction, 0.0f)));

    if (is_box) {
        return false;
    }

    float closestT = std::numeric_limits<float>::max();
    bool hit = false;

    auto& vertices = geometry_.vertices;
    auto& indices = geometry_.indices;

    for (size_t i = 0; i < indices.size(); i += 3) {
        float t;
        glm::vec3 v0 = vertices[indices[i]];
        glm::vec3 v1 = vertices[indices[i + 1]];
        glm::vec3 v2 = vertices[indices[i + 2]];

        auto intersect = ray_triangle_intersect(localRay.origin, localRay.direction, v0, v1, v2, t);

        if (intersect && t > 0.0001f) {
            if (t < closestT) {
                closestT = t;
                hit = true;
            }
        }
    }
    return hit;
}

Shape* Shape::clone()
{
	return nullptr;
}