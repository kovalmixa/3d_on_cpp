#pragma once
#include "camera_controller.h"

CameraController* CameraController::instance_ = nullptr;

CameraController* CameraController::get_instance() { return instance_ ? instance_ : instance_ = new CameraController(); }

void CameraController::process_keyboard(const sf::Event::KeyPressed* key)
{
    switch (key->code) {
        case sf::Keyboard::Key::W : { position.z -= SPEED; break; }
        case sf::Keyboard::Key::S : { position.z += SPEED; break; }
        case sf::Keyboard::Key::A : { position.x -= SPEED; break; }
        case sf::Keyboard::Key::D : { position.x += SPEED; break; }
        case sf::Keyboard::Key::Z : { position.y -= SPEED; break; }
        case sf::Keyboard::Key::X : { position.y += SPEED; break; }
    }
}
