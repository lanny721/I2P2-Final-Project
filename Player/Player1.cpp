#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>
#include <iostream>

Player::Player(const char* spriteSheetPath, float x, float y, float speed, float interval)
    : position(x, y), speed(speed), animationTimer(0), animationInterval(interval), currentFrame(0), maxFrames(3), isMoving(false) {
    spriteSheet = al_load_bitmap(spriteSheetPath);
    if (!spriteSheet) {
        throw std::runtime_error("Failed to load sprite sheet.");
    }
}
Player::~Player() {
    if (spriteSheet) {
        al_destroy_bitmap(spriteSheet);
    }
}

void Player::Update(float deltaTime) {
    if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] ||
        Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
            isMoving = true;
            if        (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y -= speed * deltaTime;
                // if (camera.y < 0) camera.y = 0;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y += speed * deltaTime;
                // if (camera.y > MapHeight * BlockSize - GetClientSize().y) camera.y = MapHeight * BlockSize - GetClientSize().y;
            }
            if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x -= speed * deltaTime;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x += speed * deltaTime;
            }
            std::cout << "Camera position: (" << Engine::GameEngine::GetInstance().GetActiveScene()->camera.x << ", " << 
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.y << ")" << std::endl;
            animationTimer += deltaTime;
            if (animationTimer >= animationInterval) {
                animationTimer = 0;
                currentFrame = (currentFrame + 1) % maxFrames; // 循環切換幀
            }
    }
}
void Player::Draw() const {
    int frameWidth = 32; // 每幀的寬度
    int frameHeight = 32; // 每幀的高度
    int row = isMoving ? 1 : 0; // 第二列為移動動畫，第一列為靜止動畫

   float scale = 1.5f; // 放大比例

    al_draw_scaled_bitmap(spriteSheet, 
                          currentFrame * frameWidth, row * frameHeight, frameWidth, frameHeight, // 原始圖片區域
                          position.x, position.y, frameWidth * scale, frameHeight * scale,      // 放大後的位置和大小
                          0);
}