#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>
#include <iostream>
#include "Engine/LOG.hpp"

Player::Player(const char* spriteSheetPath, float x, float y, float speed, float interval)
    : position(x, y), speed(speed), animationTimer(0), animationInterval(interval), currentFrame(0), maxFrames(4), isMoving(false) {
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
    cameraTicks += deltaTime;
    animationTimer += deltaTime;
    maxFrames = isMoving ? 4 : 2; // 如果正在移動，則有4幀動畫，否則只有1幀靜止動畫
    animationInterval = isMoving ? 0.1f : 0.5f; // 移動時每幀0.1秒，靜止時每幀0.5秒
    if (animationTimer >= animationInterval) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % maxFrames; // 循環切換幀
    }
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
                leftRight = false;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
                Engine::GameEngine::GetInstance().GetActiveScene()->camera.x += speed * deltaTime;
                leftRight = true;
            }
            if (cameraTicks > 1.f) {
                cameraTicks = 0.f;
                // std::cout << "Camera pos: " << Engine::GameEngine::GetInstance().GetActiveScene()->camera << std::endl;
            }
    } else {
        isMoving = false;
        maxFrames = isMoving ? 4 : 2;
        currentFrame %= maxFrames;
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
                          leftRight);
                          
}