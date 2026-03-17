#include "shape.h"
#include "basic_functions.h"

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

void Shape::draw_outline()
{
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f); // чтобы линии не мерцали (z-fighting)

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // рисуем как линии
    glLineWidth(2.5f); // толщина обводки

    glDisable(GL_DEPTH_TEST); // чтобы обводка была поверх
    glColor3f(0.f, 0.f, 0.f); // цвет обводки (черный)

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(geometry_.indices.size()),
        GL_UNSIGNED_INT,
        0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // вернуть обратно
    glDisable(GL_POLYGON_OFFSET_LINE);
}

Shape::Shape() : VAO(0), VBO(0), EBO(0) {
    shader_ = new Shader("C:/vertex.glsl", "C:/fragment.glsl");
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

void Shape::draw(sf::RenderWindow& window) {
    window.pushGLStates();
    sf::Vector2u windowSize = window.getSize();
    float aspect = static_cast<float>(windowSize.x) / std::max(static_cast<float>(windowSize.y), 1.0f);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glViewport(0, 0, windowSize.x, windowSize.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.f, aspect, 0.1f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -5.f);
    glTranslatef(transform_.position.x, transform_.position.y, transform_.position.z);
    glRotatef(transform_.rotation.x, 1, 0, 0);
    glRotatef((float)global_clock.getElapsedTime().asSeconds() * 50.f, 0, 1, 0);
    glRotatef(transform_.rotation.z, 0, 0, 1);
    glScalef(transform_.scale.x, transform_.scale.y, transform_.scale.z);
    //glColor4f(color_.r, color_.g, color_.b, color_.a);

    //shader_->use();
    //GLint colorLoc = glGetUniformLocation(shader_->program, "uColor");
    //glUniform4f(colorLoc, 1, 1, 1, 1);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(geometry_.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //draw_outline();

    window.popGLStates();
}

bool Shape::contains(const sf::Vector2f point)
{
	return false;
}

Shape* Shape::clone()
{
	return nullptr;
}