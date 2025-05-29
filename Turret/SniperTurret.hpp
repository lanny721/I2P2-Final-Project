#ifndef SNIPER_TURRET_HPP
#define SNIPER_TURRET_HPP
#include "Turret.hpp"

class SniperTurret : public Turret {
public:
    static const int Price;
    SniperTurret(float x, float y);
    void CreateBullet() override;
};
#endif // SNIPER_TURRET_HPP