#include "IScene.hpp"
#include <allegro5/allegro.h>

namespace Engine {
    void IScene::Terminate() {
        camera = Point(0, 0);
        Clear();
    }
    void IScene::Draw() const {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        Group::Draw();
    }
}
