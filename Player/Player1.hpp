#ifndef PLAYER1_HPP
#define PLAYER1_HPP
#include "Engine/Point.hpp"
#include <allegro5/allegro5.h>
namespace Engine {
    class GameEngine;
}
class PlayScene;

class Player {
private:
    float leftRightAngle = 0.f; // 左右移動時的角度
    float upDownAngle = 0.f; // 上下移動時的角度
    float scale = 1.5f; // 放大比例
    int frameWidth = 32; // 每幀的寬度
    int frameHeight = 32; // 每幀的高度
    int PlayerWidth = frameWidth * scale; // 角色圖片的寬度
    int PlayerHeight = frameHeight * scale; // 角色圖片的高度
    bool leftRight; // left: false, right: true
    bool rotate = true; // 是否旋轉
    bool isKeyMoving() const ;
    PlayScene* getPlayScene();

public:
    Engine::Point position; // 角色座標
    float speed = 400.f;            // 移動速度
    ALLEGRO_BITMAP *image;  // 角色圖片
    ALLEGRO_BITMAP* spriteSheet;
    int currentFrame; // 當前動畫幀
    int maxFrames; // 最大幀數
    float animationTimer; // 計時器
    float animationInterval; // 每幀切換的間隔
    bool isMoving; // 是否正在移動

    Player(const char* spriteSheetPath, Engine::Point initialPosition, float speed, float interval);
    ~Player();

    void Update(float deltaTime); // 更新角色位置
    void Draw() const;        // 繪製角色
    void OnKeyDown(int keyCode); // 處理按鍵事件
};

#endif // PLAYER1_HPP