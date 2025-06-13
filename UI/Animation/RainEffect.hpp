#ifndef RAINEFFECT_HPP
#define RAINEFFECT_HPP

#include "Engine/Sprite.hpp"

class RainEffect : public Engine::Sprite {
private:
    float speedY; // 雨滴的垂直速度。
    float lifetime; // 雨滴的存活時間。
    int w;
    int h;
public:
    RainEffect(float x, float y, float speedY, float lifetime);
    void Update(float deltaTime) override;
};

#endif // RAINEFFECT_HPP