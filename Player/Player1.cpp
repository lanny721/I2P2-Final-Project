#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>
#include <iostream>
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"

Player::Player(const char* spriteSheetPath, Engine::Point initialPosition, float speed, float interval)
    : position(initialPosition), speed(speed), animationTimer(0), animationInterval(interval), currentFrame(0), maxFrames(4), isMoving(false) {
    spriteSheet = al_load_bitmap(spriteSheetPath);
    if (!spriteSheet) {
        Engine::LOG(Engine::LogType::ERROR) << "Failed to load sprite sheet: " << spriteSheetPath;
    }
    Update(0); // 初始化時更新一次位置
    // Engine::GameEngine::GetInstance().GetActiveScene()->camera = position - 
    //     Engine::Point(Engine::GameEngine::GetInstance().GetScreenWidth() / 4, Engine::GameEngine::GetInstance().GetScreenHeight() / 2);
}
Player::~Player() {
    if (spriteSheet) {
        al_destroy_bitmap(spriteSheet);
    }
}

void Player::Update(float deltaTime) {
    cameraTicks += deltaTime;
    animationTimer += deltaTime;
    maxFrames = isMoving ? 4 : 2; // 如果正在移動，則有4幀動畫，否則只有2幀靜止動畫
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
                float newy = position.y - speed * deltaTime;
                if (newy >= 0 && newy <= PlayScene::MapHeight * PlayScene::BlockSize - frameHeight*scale) {
                    position.y = newy;
                } else if (newy < 0) {
                    position.y = 0;
                } else {
                    position.y = PlayScene::MapHeight * PlayScene::BlockSize - frameHeight*scale;
                }
                //position.y -= speed * deltaTime;
                // Engine::GameEngine::GetInstance().GetActiveScene()->camera.y -= speed * deltaTime;
                // if (camera.y < 0) camera.y = 0;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_DOWN]) {
                float newy = position.y + speed * deltaTime;
                if (newy >= 0 && newy <= PlayScene::MapHeight * PlayScene::BlockSize - frameHeight*scale) {
                    position.y = newy;
                } else if (newy < 0) {
                    position.y = 0;
                } else {
                    position.y = PlayScene::MapHeight * PlayScene::BlockSize - frameHeight*scale;
                }
                // Engine::GameEngine::GetInstance().GetActiveScene()->camera.y += speed * deltaTime;
                // if (camera.y > MapHeight * BlockSize - GetClientSize().y) camera.y = MapHeight * BlockSize - GetClientSize().y;
            }
            if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_LEFT]) {
                float newx = position.x - speed * deltaTime;
                if (newx >= 0 && newx <= PlayScene::MapWidth * PlayScene::BlockSize - frameWidth*scale) {
                    position.x = newx;
                } else if (newx < 0) {
                    position.x = 0;
                } else {
                    position.x = PlayScene::MapWidth * PlayScene::BlockSize- frameWidth*scale;
                }
                // Engine::GameEngine::GetInstance().GetActiveScene()->camera.x -= speed * deltaTime;
                leftRight = false;
            } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_RIGHT]) {
                float newx = position.x + speed * deltaTime;
                if (newx >= 0 && newx <= PlayScene::MapWidth * PlayScene::BlockSize - frameWidth*scale) {
                    position.x = newx;
                } else if (newx < 0) {
                    position.x = 0;
                } else {
                    position.x = PlayScene::MapWidth * PlayScene::BlockSize - frameWidth*scale;
                }
                // Engine::GameEngine::GetInstance().GetActiveScene()->camera.x += speed * deltaTime;
                leftRight = true;
            }
            if (cameraTicks > 1.f) {
                cameraTicks = 0.f;
                // std::cout << "Camera pos: " << Engine::GameEngine::GetInstance().GetActiveScene()->camera << std::endl;
            }

            Engine::GameEngine::GetInstance().GetActiveScene()->OnMouseMove(
                Engine::GameEngine::GetInstance().GetMousePosition().x, 
                Engine::GameEngine::GetInstance().GetMousePosition().y
            );
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

//    al_draw_tinted_scaled_rotated_bitmap(spriteSheet, 
//                                         al_map_rgba(255, 255, 255, 255), // 使用白色調色板
//                                         frameWidth / 2.0f, frameHeight / 2.0f, // 鏡頭中心點
//                                         position.x - Engine::GameEngine::GetInstance().GetActiveScene()->camera.x + frameWidth / 2.0f * scale, 
//                                         position.y - Engine::GameEngine::GetInstance().GetActiveScene()->camera.y + frameHeight / 2.0f * scale, 
//                                         scale, scale, // 放大後的位置和大小
//                                         0, leftRight); // 水平翻轉)
    al_draw_scaled_bitmap(spriteSheet, 
                          currentFrame * frameWidth, row * frameHeight, frameWidth, frameHeight, // 原始圖片區域
                        //   Engine::GameEngine::GetInstance().GetScreenWidth() / 4, Engine::GameEngine::GetInstance().GetScreenHeight() / 2,
                          position.x - Engine::GameEngine::GetInstance().GetActiveScene()->camera.x, 
                          position.y - Engine::GameEngine::GetInstance().GetActiveScene()->camera.y, 
                          frameWidth * scale, frameHeight * scale, // 放大後的位置和大小
                          leftRight);
                          
}
PlayScene* Player::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}