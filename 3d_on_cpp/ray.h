#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Graphics.hpp>
#include "view_projection.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() : origin(0.f), direction(0.f, 0.f, -1.f) {}

    Ray(const sf::RenderWindow& window, const sf::Vector2f& point2d, const ViewProjection& vp) {
        float x = (2.0f * point2d.x) / window.getSize().x - 1.0f;
        float y = 1.0f - (2.0f * point2d.y) / window.getSize().y;

        glm::mat4 inv_vp = glm::inverse(vp.projection * vp.view);
        glm::vec4 near_point = inv_vp * glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 far_point = inv_vp * glm::vec4(x, y, 1.0f, 1.0f);

        origin = glm::vec3(near_point) / near_point.w;
        direction = glm::normalize(glm::vec3(far_point) / far_point.w - origin);
    }
};