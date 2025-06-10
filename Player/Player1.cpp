#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>
#include <iostream>

Player::Player(const char *imagePath, float x, float y, float speed)
    : position(x, y), speed(speed) {
    image = al_load_bitmap(imagePath);
    if (!image) {
        throw std::runtime_error("Failed to load player image.");
    }
}

Player::~Player() {
    if (image) {
        al_destroy_bitmap(image);
    }
}

void Player::Update(float deltaTime) {
    if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
            if        (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y -= speed * deltaTime;
                // if (camera.y < 0) camera.y = 0;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y += speed * deltaTime;
                // if (camera.y > MapHeight * BlockSize - GetClientSize().y) camera.y = MapHeight * BlockSize - GetClientSize().y;
            } if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x -= speed * deltaTime;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x += speed * deltaTime;
            }
            std::cout << "Camera position: (" << Engine::GameEngine::GetInstance().GetActiveScene()->camera.x << ", " << 
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y << ")" << std::endl;
    }
}
void Player::Draw() const {
    al_draw_bitmap(image, position.x, position.y, 0);
}