#ifndef SANDBOX_HPP
#define SANDBOX_HPP
#include <string>
#include <vector>
#include "Turret.hpp"

class SandBox : public Turret {
public:
    SandBox(float x, float y, std::string path);
    void Update(float deltaTime) override;
    void Draw() const override ;
    void CreateBullet() override {};
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps; // Store the bitmaps for the sandbox turret.
};
#endif // SANDBOX_HPP