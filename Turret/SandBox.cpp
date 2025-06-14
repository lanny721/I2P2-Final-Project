#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "SandBox.hpp"
#include "Scene/PlayScene.hpp"

SandBox::SandBox(float x, float y, std::string path): Turret(path, path, x, y, 0, 0, 0.0) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void SandBox::CreateBullet() {};