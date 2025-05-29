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
    Engine::Point diffs[5];
    for(int i=1;i<=4;++i) {
        diffs[i].x =  sin(Rotation + i * 2*ALLEGRO_PI/5);
        diffs[i].y = -cos(Rotation + i * 2*ALLEGRO_PI/5);
    } 
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    for(int i=1;i<=4;++i) getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position, diffs[i], rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
