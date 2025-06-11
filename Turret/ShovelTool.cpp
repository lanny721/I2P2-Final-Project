#include <allegro5/base.h>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "ShovelTool.hpp"
#include "Scene/PlayScene.hpp"

#include "Engine/GameEngine.hpp"
#include <iostream>
#include "Enemy/Enemy.hpp"

const int ShovelTool::Price = 0;
ShovelTool::ShovelTool(float x, float y): Turret("play/shovel-base.png", "play/shovel.png", x, y, 0, Price, INFINITY) {
    Anchor.y += 8.0f / GetBitmapHeight();
    isTool = true;
}
void ShovelTool::Use() {
    int x = floor(Position.x / PlayScene::BlockSize);
    int y = floor(Position.y / PlayScene::BlockSize);
    if (getPlayScene()->mapState[y][x] == PlayScene::TILE_FLOOR) { // dig to dirt

        getPlayScene()->mapState[y][x] = PlayScene::TILE_DIRT;
        getPlayScene()->TileMapGroup->RemoveObject(getPlayScene()->TileMapImages[y][x]->GetObjectIterator());
        getPlayScene()->TileMapImages[y][x] = new Engine::Image("play/dirt.png", x * PlayScene::BlockSize, y * PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize);
        getPlayScene()->TileMapImages[y][x]->followCamera = true;
        getPlayScene()->TileMapGroup->AddNewObject(getPlayScene()->TileMapImages[y][x]);
        getPlayScene()->mapDistance = getPlayScene()->CalculateBFSDistance();
        // Update Enemy Path
        for (auto &it : getPlayScene()->EnemyGroup->GetObjects()) {
            dynamic_cast<Enemy *>(it)->UpdatePath(getPlayScene()->mapDistance);
        }
    } 
    else if (getPlayScene()->mapState[y][x] == PlayScene::TILE_OCCUPIED) { // remove tower
        getPlayScene()->mapState[y][x] = PlayScene::TILE_FLOOR;
        getPlayScene()->TowerGroup->RemoveObject(getPlayScene()->Towers[y][x]->GetObjectIterator());
        getPlayScene()->EarnMoney(getPlayScene()->Towers[y][x]->GetPrice());
        getPlayScene()->Towers[y][x] = nullptr;
    }
}
