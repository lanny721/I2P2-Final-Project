#ifndef REDCAR_ENEMY_HPP
#define REDCAR_ENEMY_HPP
#include "Enemy.hpp"

class RedcarEnemy : public Enemy {
public:
    RedcarEnemy(int x, int y);
    void feature() override;
};
#endif // REDCAR_ENEMY_HPP
