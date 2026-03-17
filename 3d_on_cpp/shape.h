#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "aabb.h"
#include "transform.h"
#include "geometry.h"
#include "shader.h"

class Shape {
private:
    GLuint VAO, VBO, EBO;
    sf::Clock global_clock;
    Shader* shader_;
    Geometry geometry_;
    Transform transform_;
    glm::vec4 color_;
    AABB bounding_box_;

    void apply_transform();
    void set_bounding_box();

    void draw_outline();
public:
    Shape();
    ~Shape();

    friend void to_json(nlohmann::json& j, const Shape& s);
    friend void from_json(const nlohmann::json& j, Shape& s);

    void set_geometry(const Geometry& geometry);
    void set_transform(const Transform& transform_data);
    Transform get_transform() const;

    void set_outline(const bool show);
    glm::vec4 get_color();
    void set_color(const glm::vec4& color);
    AABB get_box();

    void draw(sf::RenderWindow& window);
    bool contains(const sf::Vector2f point);
    Shape* clone();
};

inline void to_json(nlohmann::json& j, const Shape& s) {
    j = nlohmann::json{
        {"transform", s.transform_},
        {"geometry", s.geometry_},
        {"color", s.color_}
    };
}

inline void from_json(const nlohmann::json& j, Shape& s) {
    Transform temp_trans;
    Geometry temp_geom;
    glm::vec4 temp_color;

    j.at("transform").get_to(temp_trans);
    j.at("geometry").get_to(temp_geom);

    if (j.contains("color")) j.at("color").get_to(temp_color);
    
    s.set_transform(temp_trans);
    s.set_geometry(temp_geom);
    s.set_color(temp_color);
}