#ifndef SHOVEL_TOOL_HPP
#define SHOVEL_TOOL_HPP
#include "Turret.hpp"

class ShovelTool : public Turret {
public:
    static const int Price;
    ShovelTool(float x, float y);
    void CreateBullet() override {};

    void Use() override;
};

#endif // SHOVEL_TOOL_HPP

/*
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
*/
