#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "SniperTurret.hpp"
#include "Scene/PlayScene.hpp"

const int SniperTurret::Price = 300;
SniperTurret::SniperTurret(float x, float y): Turret("play/tower-base.png", "play/turret-7.png", x, y, 500, Price, 0.25) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void SniperTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(sin(Rotation), -cos(Rotation));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
