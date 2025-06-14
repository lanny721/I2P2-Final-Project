#ifndef SANDBOX_HPP
#define SANDBOX_HPP
#include <string>
#include "Turret.hpp"

class SandBox : public Turret {
public:
    SandBox(float x, float y, std::string path);
    void CreateBullet() override {};
};
#endif // SANDBOX_HPP