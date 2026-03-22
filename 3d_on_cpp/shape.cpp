#include "shape.h"

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

void Shape::draw_outline(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
    if (!shader_controller_->try_use()) return;

    shader_controller_->set_uniform(projection, view, model);
    GLint colorLoc = glGetUniformLocation(shader_controller_->program, "u_color");
    glUniform4f(colorLoc, 0.f, 0.f, 0.f, 1.f);

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.f);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(geometry_.indices.size()),
        GL_UNSIGNED_INT,
        0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
}

Shape::Shape() : VAO(0), VBO(0), EBO(0) {
    shader_controller_ = ShaderController::get_instance();
    color_ = glm::vec4(1.f);
    transform_.position = glm::vec3(0.0f);
    transform_.rotation = glm::vec3(0.0f);
    transform_.scale = glm::vec3(1.0f);
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

void Shape::draw(sf::RenderWindow& window, glm::vec3 camera_pos)
{
    window.resetGLStates();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sf::Vector2u windowSize = window.getSize();
    glViewport(0, 0, windowSize.x, windowSize.y);

    float aspect = static_cast<float>(windowSize.x) /
        std::max(static_cast<float>(windowSize.y), 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(camera_pos, camera_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform_.position);
    model = glm::rotate(model, glm::radians(transform_.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, (float)global_clock.getElapsedTime().asSeconds(), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform_.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, transform_.scale);

    shader_status = shader_controller_->try_use();
    if (!shader_status) return;

    shader_controller_->set_uniform(projection, view, model);

    GLint colorLoc = glGetUniformLocation(shader_controller_->program, "u_color");

    glUniform4f(colorLoc, color_.r, color_.g, color_.b, color_.a);
    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(geometry_.indices.size()),
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
    //draw_outline(projection, view, model);
}

bool Shape::contains(const sf::Vector2f point)
{
	return false;
}

Shape* Shape::clone()
{
	return nullptr;
}