#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ViewProjection {
	glm::mat4 projection;
	glm::mat4 view;
	bool is_perspective;

	ViewProjection(sf::RenderWindow& window, glm::vec3 camera_pos, bool _is_perspective) {
		sf::Vector2u windowSize = window.getSize();
		float aspect = static_cast<float>(windowSize.x) /
			std::max(static_cast<float>(windowSize.y), 1.0f);
		is_perspective = _is_perspective;
		if (is_perspective) projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		else {
			float size = 5.0f;
			projection = glm::ortho(-size * aspect, size * aspect, -size, size, 0.1f, 100.0f);
		}
		view = glm::lookAt(camera_pos, camera_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
};