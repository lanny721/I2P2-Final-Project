#include <allegro5/base.h>
#include <random>
#include <string>

#include "Engine/Point.hpp"
#include "PlaneEnemy.hpp"

PlaneEnemy::PlaneEnemy(int x, int y)
    : Enemy("play/enemy-2.png", x, y, 10, 75, 5, 5),
      targetRotation(0) {
}
void PlaneEnemy::Draw() const {
    Enemy::Draw();
}
void PlaneEnemy::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    // Choose arbitrary one.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(0.0f, 4.0f);
    float rnd = dist(rng);
    if (rnd < deltaTime) {
        // Head arbitrary rotation.
        std::uniform_real_distribution<> distRadian(-ALLEGRO_PI, ALLEGRO_PI);
        targetRotation = distRadian(rng);
    }
}
