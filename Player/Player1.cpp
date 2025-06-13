#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
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
    Engine::GameEngine::GetInstance().GetActiveScene()->camera = position - 
        Engine::Point(PlayScene::defW * PlayScene::BlockSize / 2.0f, PlayScene::defH * PlayScene::BlockSize / 2.0f);
}
Player::~Player() {
    if (spriteSheet) {
        al_destroy_bitmap(spriteSheet);
    }
}
void Player::Update(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= animationInterval) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % maxFrames; // 循環切換幀
    }
    if (isKeyMoving()) {
        isMoving = true;
        if      (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP]   ) movingSpeed.y = -speed;
        else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN] ) movingSpeed.y =  speed;
        if      (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] ) movingSpeed.x = -speed;
        else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) movingSpeed.x =  speed;
    } else isMoving = false;
    if (movingSpeed.Magnitude() != 0.f) {
        int upBound = PlayerHeight / 2;
        int downBound = PlayScene::MapHeight * PlayScene::BlockSize - PlayerHeight / 2;
        int leftBound = PlayerWidth / 2;
        int rightBound = PlayScene::MapWidth * PlayScene::BlockSize - PlayerWidth / 2;

        Engine::Point newPos = position;
        // if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || 
        //     Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
        if (movingSpeed.y != 0.f) {
                newPos.y = position.y + movingSpeed.y * deltaTime;
                if (newPos.y >= upBound && newPos.y <= downBound) { // since draw in center
                    if(getPlayScene()->canWalk(newPos / PlayScene::BlockSize)) {
                        upDownAngle += (newPos.y - position.y) / (float)(frameHeight / 2);
                        position = newPos;
                    } 
                } else if (newPos.y < upBound) position.y = upBound;
                  else position.y = downBound;
        } 
        newPos = position;
        // if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] || 
        //     Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
        if (movingSpeed.x != 0.f) {
                leftRight = movingSpeed.x > 0.f; // 判斷左右移動
                newPos.x = position.x + movingSpeed.x * deltaTime;
                if (newPos.x >= leftBound && newPos.x <= rightBound) {
                    if(getPlayScene()->canWalk(newPos / PlayScene::BlockSize)) {
                        leftRightAngle += (newPos.x - position.x) / (((float)frameWidth) / 1.2f);
                        position = newPos;
                    } 
                } else if (newPos.x < leftBound) position.x = leftBound;
                  else position.x = rightBound;
        } 
        if (abs(movingSpeed.x) >= abs(movingSpeed.y)) upDownAngle = 0.f; // Reset upDownAngle when moving left/right

        Engine::GameEngine::GetInstance().GetActiveScene()->OnMouseMove(
            Engine::GameEngine::GetInstance().GetMousePosition().x, 
            Engine::GameEngine::GetInstance().GetMousePosition().y);
    } 
    movingSpeed = movingSpeed * powf(getPlayScene()->isRaining || 
        getPlayScene()->mapState[position.y/PlayScene::BlockSize][position.x/PlayScene::BlockSize] == PlayScene::TILE_WATER ? 
        0.02f : 0.001f, deltaTime);
    if (abs(movingSpeed.x) < 30.f) movingSpeed.x = 0.f;
    if (abs(movingSpeed.y) < 30.f) movingSpeed.y = 0.f;
    if (getPlayScene()->mapState[position.y/PlayScene::BlockSize][position.x/PlayScene::BlockSize] == PlayScene::TILE_DOOR) position = PlayScene::EndGridPoint * PlayScene::BlockSize;

    maxFrames = isMoving ? 4 : 2; // 如果正在移動，則有4幀動畫，否則只有2幀靜止動畫
    animationInterval = isMoving ? 0.1f : 0.5f; // 移動時每幀0.1秒，靜止時每幀0.5秒
    currentFrame %= maxFrames;
    
    Engine::Point Displacement = position - 
        Engine::Point(PlayScene::defW * PlayScene::BlockSize / 2.0f, 
                      PlayScene::defH * PlayScene::BlockSize / 2.0f) - 
        Engine::GameEngine::GetInstance().GetActiveScene()->camera; // 位移 (end - start)
    Engine::GameEngine::GetInstance().GetActiveScene()->camera = 
        Engine::GameEngine::GetInstance().GetActiveScene()->camera + 
        Displacement * //0.15f;
        0.2f * (1.f - powf(1.01f, -Displacement.Magnitude()));
    if (Engine::GameEngine::GetInstance().GetActiveScene()->camera.x < -200) Engine::GameEngine::GetInstance().GetActiveScene()->camera.x = -200;
    if (Engine::GameEngine::GetInstance().GetActiveScene()->camera.x > (PlayScene::MapWidth - PlayScene::defW) * PlayScene::BlockSize + 200) 
        Engine::GameEngine::GetInstance().GetActiveScene()->camera.x = (PlayScene::MapWidth - PlayScene::defW) * PlayScene::BlockSize + 200;
    if (Engine::GameEngine::GetInstance().GetActiveScene()->camera.y < -200) Engine::GameEngine::GetInstance().GetActiveScene()->camera.y = -200;
    if (Engine::GameEngine::GetInstance().GetActiveScene()->camera.y > (PlayScene::MapHeight - PlayScene::defH) * PlayScene::BlockSize + 200) 
        Engine::GameEngine::GetInstance().GetActiveScene()->camera.y = (PlayScene::MapHeight - PlayScene::defH) * PlayScene::BlockSize + 200;
    // Engine::GameEngine::GetInstance().GetActiveScene()->camera = position - 
    //     Engine::Point(PlayScene::defW * PlayScene::BlockSize / 2.0f, 
    //                   PlayScene::defH * PlayScene::BlockSize / 2.0f);
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
    if (getPlayScene()->DebugMode) al_draw_filled_circle(position.x - getPlayScene()->camera.x, position.y - getPlayScene()->camera.y, 
                                                         5 * PlayScene::BlockSize, al_map_rgba(200, 255, 200, 150));
}
void Player::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_SPACE) rotate = !rotate; // 按空格鍵切換旋轉狀態
}
PlayScene* Player::getPlayScene() const {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
bool Player::isKeyMoving() const {
    return Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_UP] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT] ||
           Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN];
}
int Player::getGridPosX() const {
    return position.x / PlayScene::BlockSize;
}
int Player::getGridPosY() const {
    return position.y / PlayScene::BlockSize;
}
