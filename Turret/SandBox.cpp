#include <allegro5/base.h>
#include <cmath>
#include <string>
#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "SandBox.hpp"
#include "Scene/PlayScene.hpp"

SandBox::SandBox(float x, float y, std::string path): Turret(path, path, x, y, 0, 0, 0.0) {
    Anchor.y += 8.0f / GetBitmapHeight();
    bmps = std::vector<std::shared_ptr<ALLEGRO_BITMAP>>(7);
    bmps[0] = Engine::Resources::GetInstance().GetBitmap("play/grass2.png", Size.x, Size.y);
    bmps[1] = Engine::Resources::GetInstance().GetBitmap("play/rock.png", Size.x, Size.y);
    bmps[2] = Engine::Resources::GetInstance().GetBitmap("play/rock_grass.png", Size.x, Size.y);
    bmps[3] = Engine::Resources::GetInstance().GetBitmap("play/flowers3.png", Size.x, Size.y);
    bmps[4] = Engine::Resources::GetInstance().GetBitmap("play/water.png", Size.x, Size.y);
    bmps[5] = Engine::Resources::GetInstance().GetBitmap("play/gold.png", Size.x, Size.y);
    bmps[6] = Engine::Resources::GetInstance().GetBitmap("play/door.png", Size.x, Size.y);
}
    // if (id == 0 )
    //     new_preview = new SandBox(0, 0, "play/grass2.png"); //grass
    // else if (id == 1)
    //     new_preview = new SandBox(0, 0, "play/rock.png"); //rock
    // else if (id == 2)
    //     new_preview = new SandBox(0, 0, "play/rock_grass.png"); //rock_grass
    // else if (id == 3 )
    //     new_preview = new SandBox(0, 0, "play/flowers3.png"); //flower
    // else if (id == 4 )
    //     new_preview = new SandBox(0, 0, "play/water.png"); //water
    // else if (id == 5 )
    //     new_preview = new SandBox(0, 0, "play/gold.png"); //gold
    // else if (id == 6 )
    //     new_preview = new SandBox(0, 0, "play/door.png"); //door
