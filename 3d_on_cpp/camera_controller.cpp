#pragma once
#include "camera_controller.h"

CameraController* CameraController::instance_ = nullptr;

CameraController* CameraController::get_instance() { return instance_ ? instance_ : instance_ = new CameraController(); }

void CameraController::process_keyboard(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) position.z -= SPEED * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) position.z += SPEED * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) position.x -= SPEED * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) position.x += SPEED * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) position.y -= SPEED * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) position.y += SPEED * dt;
}
