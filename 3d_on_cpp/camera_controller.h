#pragma once
#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>

class CameraController {
private:
	static CameraController* instance_;

	const float SPEED = 0.05f;

	CameraController() = default;
	~CameraController() = default;
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);

	static CameraController* get_instance();

	void process_keyboard(const sf::Event::KeyPressed* key);
};