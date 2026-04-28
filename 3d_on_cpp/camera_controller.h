#pragma once
#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>

class CameraController {
private:
	static CameraController* instance_;

	bool is_moving = false;
	const float SPEED = 3.f;

	CameraController() = default;
	~CameraController() = default;
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);

	static CameraController* get_instance();

	void switch_movement();
	void key_movement(const float dt);
};