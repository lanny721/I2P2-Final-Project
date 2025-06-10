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
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D]) {
            if        (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W]) {
                // position.y -= speed * deltaTime;
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y -= speed * deltaTime;
                // if (camera.y < 0) camera.y = 0;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S]) {
                // position.y += speed * deltaTime;
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y += speed * deltaTime;
                // if (camera.y > MapHeight * BlockSize - GetClientSize().y) camera.y = MapHeight * BlockSize - GetClientSize().y;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A]) {
                // position.x -= speed * deltaTime;
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x -= speed * deltaTime;
                // if (camera.x < 0) camera.x = 0;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D]) {
                // position.x += speed * deltaTime;
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x += speed * deltaTime;
                // if (camera.x > MapWidth * BlockSize - GetClientSize().x) camera.x = MapWidth * BlockSize - GetClientSize().x;
            }
            std::cout << "Position: (" << position.x << ", " << position.y << ")" << std::endl;
    }
}
void Player::Draw() const {
    al_draw_bitmap(image, position.x, position.y, 0);
}