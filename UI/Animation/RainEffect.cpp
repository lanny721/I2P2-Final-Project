#include "RainEffect.hpp"
#include "Engine/GameEngine.hpp"
RainEffect::RainEffect(float x, float y, float speedY, float lifetime)
    : Sprite("play/raindrop.png", x, y), speedY(speedY), lifetime(lifetime) {}

void RainEffect::Update(float deltaTime) {
    Position.y += speedY * deltaTime; // 雨滴向下移動。
    lifetime -= deltaTime;
    if (lifetime <= 0 || Position.y > Engine::GameEngine::GetInstance().GetScreenHeight()) {
        // 如果雨滴超出屏幕或存活時間結束，刪除雨滴。
        GetObjectIterator()->first = false;
    }
    
}