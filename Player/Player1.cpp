#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>
#include <iostream>
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include <cmath>

Player::Player(const char* spriteSheetPath, Engine::Point initialPosition, float speed, float interval)
    : position(initialPosition), speed(speed), animationTimer(0), animationInterval(interval), currentFrame(0), maxFrames(4), isMoving(false) {
    spriteSheet = al_load_bitmap(spriteSheetPath);
    if (!spriteSheet) {
        Engine::LOG(Engine::LogType::ERROR) << "Failed to load sprite sheet: " << spriteSheetPath;
    }
    Update(0); // 初始化時更新一次位置
}
Player::~Player() {
    if (spriteSheet) {
        al_destroy_bitmap(spriteSheet);
    }
}
void Player::Update(float deltaTime) {
    animationTimer += deltaTime;
    maxFrames = isMoving ? 4 : 2; // 如果正在移動，則有4幀動畫，否則只有2幀靜止動畫
    animationInterval = isMoving ? 0.1f : 0.5f; // 移動時每幀0.1秒，靜止時每幀0.5秒
    if (animationTimer >= animationInterval) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % maxFrames; // 循環切換幀
    }

    if (isKeyMoving()) {
        isMoving = true;
        int upBound = PlayerHeight / 2;
        int downBound = PlayScene::MapHeight * PlayScene::BlockSize - PlayerHeight / 2;
        int leftBound = PlayerWidth / 2;
        int rightBound = PlayScene::MapWidth * PlayScene::BlockSize - PlayerWidth / 2;

        if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || 
            Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
                float newy = position.y + speed * deltaTime *
                    (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN] ? 1 : -1);
                int targetGridY = (int)(newy / PlayScene::BlockSize);
                int targetGridX = (int)(position.x / PlayScene::BlockSize);
                if (newy >= upBound && newy <= downBound) { // since draw in center
                    if(getPlayScene()->canWalk(targetGridY, targetGridX)) {
                        upDownAngle += (newy - position.y) / (float)(frameHeight / 2);
                        position.y = newy;
                    } 
                } else if (newy < upBound) position.y = upBound;
                  else position.y = downBound;
        } 
        else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] || 
            Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
                leftRight = Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]; // 判斷左右移動
                float newx = position.x + speed * deltaTime * (leftRight ? 1 : -1); // 根據左右移動決定x座標增量
                int targetGridY = (int)(position.y / PlayScene::BlockSize);
                int targetGridX = (int)(newx / PlayScene::BlockSize);
                if (newx >= leftBound && newx <= rightBound) {
                    if(getPlayScene()->canWalk(targetGridY, targetGridX)) {
                        upDownAngle = 0.f; // Reset upDownAngle when moving left/right
                        leftRightAngle += (newx - position.x) / (float)(frameWidth / 2);
                        position.x = newx;
                    } 
                } else if (newx < leftBound) position.x = leftBound;
                  else position.x = rightBound;
        } 

        Engine::GameEngine::GetInstance().GetActiveScene()->OnMouseMove(
            Engine::GameEngine::GetInstance().GetMousePosition().x, 
            Engine::GameEngine::GetInstance().GetMousePosition().y);
    } else {
        isMoving = false;
        maxFrames = isMoving ? 4 : 2;
        currentFrame %= maxFrames;
    }
    
    Engine::GameEngine::GetInstance().GetActiveScene()->camera = position - 
        Engine::Point(PlayScene::defW * PlayScene::BlockSize / 2.0f, PlayScene::defH * PlayScene::BlockSize / 2.0f);
}
void Player::Draw() const {
    int row = isMoving ? 1 : 0; // 第二列為移動動畫，第一列為靜止動畫
    // press space to rotate!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    al_draw_tinted_scaled_rotated_bitmap_region(spriteSheet,
                                                currentFrame * frameWidth, row * frameHeight, frameWidth, frameHeight, // 原始圖片區域
                                                al_map_rgba(255, 255, 255, 255), // 使用白色作為顏色調整
                                                frameWidth / 2.0f, frameHeight / 2.0f, // 旋轉中心點
                                                position.x - Engine::GameEngine::GetInstance().GetActiveScene()->camera.x, 
                                                position.y - Engine::GameEngine::GetInstance().GetActiveScene()->camera.y, 
                                                scale, scale * (rotate ? cos(upDownAngle) : 1), // 放大後的位置和大小
                                                rotate * leftRightAngle, leftRight);

    // al_draw_scaled_bitmap(spriteSheet, 
    //                       currentFrame * frameWidth, row * frameHeight, frameWidth, frameHeight, // 原始圖片區域
    //                       position.x - Engine::GameEngine::GetInstance().GetActiveScene()->camera.x, 
    //                       position.y - Engine::GameEngine::GetInstance().GetActiveScene()->camera.y, 
    //                       frameWidth * scale, frameHeight * scale, // 放大後的位置和大小
    //                       leftRight);
                          
}
void Player::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_SPACE) rotate = !rotate; // 按空格鍵切換旋轉狀態
}
PlayScene* Player::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
bool Player::isKeyMoving() const {
    return Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN];
}
