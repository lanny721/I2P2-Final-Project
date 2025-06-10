#include "Player1.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdexcept>

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

void Player::Update(int keyCode) {
    // // 使用按鍵控制角色移動
    // switch (keyCode) {
    // case ALLEGRO_KEY_W: // 向上移動
    //     position.y -= speed;
    //     break;
    // case ALLEGRO_KEY_S: // 向下移動
    //     position.y += speed;
    //     break;
    // case ALLEGRO_KEY_A: // 向左移動
    //     position.x -= speed;
    //     break;
    // case ALLEGRO_KEY_D: // 向右移動
    //     position.x += speed;
    //     break;
    // }

    // // 確保角色不超出螢幕邊界
    // position.x = std::max(0.0f, std::min(position.x, 1280.0f - al_get_bitmap_width(image))); // 假設螢幕寬度為 1280
    // position.y = std::max(0.0f, std::min(position.y, 832.0f - al_get_bitmap_height(image))); // 假設螢幕高度為 832
}
void Player::Draw() const {
    al_draw_bitmap(image, position.x, position.y, 0);
}