#include <string>
#include "RedcarEnemy.hpp"

RedcarEnemy::RedcarEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 10, 100, 10, 40) {
}
void RedcarEnemy::feature() {
    speed += 5.f;
}