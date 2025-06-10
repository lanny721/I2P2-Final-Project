#ifndef PLAYER1_HPP
#define PLAYER1_HPP
#include "Engine/Point.hpp"
#include <allegro5/allegro5.h>

class Player
{
public:
    Engine::Point position; // 角色座標
    float speed;            // 移動速度
    ALLEGRO_BITMAP *image;  // 角色圖片

    Player(const char *imagePath, float x, float y, float speed);
    ~Player();

    void Update(int keyCode); // 更新角色位置
    void Draw() const;        // 繪製角色
};

#endif // PLAYER1_HPP