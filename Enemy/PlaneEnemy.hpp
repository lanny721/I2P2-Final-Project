#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class PlaneEnemy : public Enemy {
private:
    float targetRotation;

public:
    PlaneEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
};

#endif   // PLANEENEMY_HPP