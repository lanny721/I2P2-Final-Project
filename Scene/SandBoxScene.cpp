#include "Scene/SandBoxScene.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"
#include <functional>
#include "UI/Component/Label.hpp"
#include "Turret/SandBox.hpp"
#include "Turret/Turret.hpp"
#include "Turret/TurretButton.hpp"
#include "Engine/GameEngine.hpp"
#include <iostream>
#include <fstream>
#include <string>

void SandBoxScene::Initialize() {
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    imgTarget->followCamera = true;
    AddNewObject(imgTarget);
    AddNewObject(TileMapGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    mapState = std::vector<std::vector<PlayScene::TileType>>(MapHeight, std::vector<PlayScene::TileType>(MapWidth, PlayScene::TileType::TILE_DIRT));
    TileMapImages = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
    for(int i=0;i<MapHeight;i++) {
        for(int j=0;j<MapWidth;j++) {
            TileMapImages[i][j] = new Engine::Image("play/grass2.png", j * PlayScene::BlockSize, i * PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize);
            TileMapGroup->AddNewObject(TileMapImages[i][j]);
        }
    }
    ConstructUI();
}
void SandBoxScene::Terminate() {
    IScene::Terminate();
}
void SandBoxScene::Update(float deltaTime) {

}
void SandBoxScene::Draw() const {
    IScene::Draw();
}
void SandBoxScene::SaveOnClick(int stage) {
    std::string filename = "Resource/mapNumber.txt";
    std::ifstream fin(filename);
    int mapNumber ;
    fin >> mapNumber;
    fin.close();
    std::ofstream fout(filename);
    fout << ++mapNumber;
    fout.close();

    std::string mapFileName = "Resource/map" + std::to_string(mapNumber) + ".txt";
    std::ofstream mapFile(mapFileName);
    for(int i=0;i<MapHeight;i++) {
        for(int j=0;j<MapWidth;j++) {
            if(mapState[i][j] == PlayScene::TileType::TILE_DIRT) mapFile << '0';
            else if(mapState[i][j] == PlayScene::TileType::TILE_FLOOR) mapFile << '1';
            else if(mapState[i][j] == PlayScene::TileType::TILE_STONE) mapFile << '2';
            else if(mapState[i][j] == PlayScene::TileType::TILE_FLOWER) mapFile << '3';
            else if(mapState[i][j] == PlayScene::TileType::TILE_WATER) mapFile << '4';
            else if(mapState[i][j] == PlayScene::TileType::TILE_GOLD) mapFile << '5';
            else if(mapState[i][j] == PlayScene::TileType::TILE_DOOR) mapFile << '6';
            else{
                printf("Unknown tile type at (%d, %d): %d\n", i, j, mapState[i][j]);
                exit(1);// Unknown tile
            }
        }
        mapFile << '\n';
    }
    mapFile.close();
}
void SandBoxScene::OnKeyDown(int keyCode) {

}
// void SandBoxScene::OnMouseDown(int button, int mx, int my) {
//     IScene::OnMouseDown(button, mx, my);
// }
// void SandBoxScene::OnMouseMove(int mx, int my) {
//     IScene::OnMouseMove(mx, my);
// }
// void SandBoxScene::OnMouseUp(int button, int mx, int my) {
//     IScene::OnMouseUp(button, mx, my);
// }
void SandBoxScene::ConstructUI() {
    // Background
    Engine::Image* boundary=new Engine::Image("play/sand.png", uiBoundaryX, 0, 320, 832);
    UIGroup->AddNewObject(boundary);
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("sandbox"), "pirulen.ttf", 32, uiBoundaryX + 14, 20));

    Engine::ImageButton *btn;
    const int bs=76;// Button size
    // Button 1 grass
    btn = new Engine::ImageButton("play/grass2.png", "play/grass2.png", uiBoundaryX + 14, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 2 rock
    btn = new Engine::ImageButton("play/rock.png", "play/rock.png", uiBoundaryX + 14 + bs, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    //Button 3 rock+grass (obstacle)
    btn = new Engine::ImageButton("play/rock_grass.png", "play/rock_grass.png", uiBoundaryX + 14 + bs * 2, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);

    //Button 4 flower
    btn = new Engine::ImageButton("play/flowers3.png", "play/flowers3.png", uiBoundaryX + 14 + bs*3, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);

    //Button 5 water
    btn = new Engine::ImageButton("play/water.png", "play/water.png", uiBoundaryX + 14 , 80 + bs, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);

    //Button 6 gold
    btn = new Engine::ImageButton("play/gold.png", "play/gold.png", uiBoundaryX + 14 + bs , 80 + bs, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 5));
    UIGroup->AddNewControlObject(btn);

    //Button 7 door
    btn = new Engine::ImageButton("play/door.png", "play/door.png", uiBoundaryX + 14 + bs*2, 80 + bs, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 6));
    UIGroup->AddNewControlObject(btn);
}
void SandBoxScene::UIBtnClicked(int id) {
    Turret* new_preview = nullptr;
    if (id == 0 )
        new_preview = new SandBox(0, 0, "play/grass2.png"); //grass
    else if (id == 1)
        new_preview = new SandBox(0, 0, "play/rock.png"); //rock
    else if (id == 2)
        new_preview = new SandBox(0, 0, "play/rock_grass.png"); //rock_grass
    else if (id == 3 )
        new_preview = new SandBox(0, 0, "play/flowers3.png"); //flower
    else if (id == 4 )
        new_preview = new SandBox(0, 0, "play/water.png"); //water
    else if (id == 5 )
        new_preview = new SandBox(0, 0, "play/gold.png"); //gold
    else if (id == 6 )
        new_preview = new SandBox(0, 0, "play/door.png"); //door

    if (!new_preview) return;
    if(previewBox) UIGroup->RemoveObject(previewBox->GetObjectIterator());
    previewBox = new_preview;

    previewBox->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    previewBox->Tint = al_map_rgba(255, 255, 255, 200);
    previewBox->Enabled = false;
    previewBox->Preview = true;
    UIGroup->AddNewObject(previewBox);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}
// void PlayScene::readMapTiles(int y, int x, char c) {
//     if (c == '\0') Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: empty tile at " << Engine::Point(x, y);
//     else if (c == '0') {
//         mapState[y][x] = TILE_DIRT;
//         TileMapImages[y][x] = (new Engine::Image("play/grass2.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '1') {
//         mapState[y][x] = TILE_FLOOR;
//         TileMapImages[y][x] = (new Engine::Image("play/rock.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '2') {
//         mapState[y][x] = TILE_OBSTACLE;
//         TileMapImages[y][x] = (new Engine::Image("play/rock_grass.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '3') {
//         mapState[y][x] = TILE_OBSTACLE;
//         TileMapImages[y][x] = (new Engine::Image("play/flowers3.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '4') {
//         mapState[y][x] = TILE_WATER;
//         TileMapImages[y][x] = (new Engine::Image("play/water.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '5') {
//         mapState[y][x] = TILE_GOLD;
//         TileMapImages[y][x] = (new Engine::Image("play/gold.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
//     } else if (c == '6') {
//         mapState[y][x] = TILE_DOOR;
//         TileMapImages[y][x] = (new Engine::Image("play/door.png", (x - 1.5f / 7) * BlockSize, (y - 0.4f) * BlockSize, 1.5 * BlockSize, 1.5 * BlockSize));
//     }
// }
void SandBoxScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && previewBox) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        previewBox = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);

    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if (canReachInteract(x, y, mx, my)) {
        if (mapState[y][x] == TILE_GOLD) {
            ++golds;
            UIGoldLabel->Text = std::to_string(golds);
        }
        mapState[y][x] = PlayScene::TILE_DIRT;
        TileMapGroup->RemoveObject(TileMapImages[y][x]->GetObjectIterator());
        TileMapImages[y][x] = new Engine::Image("play/grass2.png", x * PlayScene::BlockSize, y * PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize);
        TileMapImages[y][x]->followCamera = true;
        TileMapGroup->AddNewObject(TileMapImages[y][x]);
    }
}
void SandBoxScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if (!previewBox || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight || mx >= uiBoundaryX) {
        imgTarget->Visible = false;
    } else {
        imgTarget->Visible = true;
        imgTarget->Position.x = x * BlockSize;
        imgTarget->Position.y = y * BlockSize;
    }
    for (int i = -3; i <= 3; i++) {
        for (int j=-3;j<=3;j++) {
            int nx=x+i,ny=y+j;
            if(nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight || mx >= uiBoundaryX) continue;
            TileMapImages[ny][nx]->color = al_map_rgb(255, 255, 255);
        }
    }
    if (canReachInteract(x, y, mx, my)) {
        // Engine::LOG(Engine::LogType::INFO) << x << ' ' << y << ' ' << "canInteract";
        // std::cout << x * BlockSize - camera.x << std::endl;
        TileMapImages[y][x]->color = al_map_rgb(200, 200, 255);
    }
}
void SandBoxScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    putThings(button, mx, my);
}
void SandBoxScene::putThings(int button, int mx, int my) {
    if (!imgTarget->Visible)
        return;
    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if ((button & 1) && mx < uiBoundaryX) {
        if (mapState[y][x] == TILE_FLOOR || preview->isTool || 1) { // deal in CheckSpaceValid
            if (!previewBox)
                return;
            //Turret* newpreview = preview;
            // Remove Preview.
            previewBox->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(previewBox->GetObjectIterator());
            // Construct real turret.
            previewBox->Position.x = x * BlockSize + BlockSize / 2;
            previewBox->Position.y = y * BlockSize + BlockSize / 2;
            previewBox->Enabled = true;
            previewBox->Preview = false;
            previewBox->Tint = al_map_rgba(255, 255, 255, 255);

            previewBox->followCamera = previewBox->imgBase.followCamera = true;
            if (!previewBox->isTool) {
                TowerGroup->AddNewObject(previewBox);
                Towers[y][x] = previewBox;
            } else {
                previewBox->Use();
            }
            // To keep responding when paused.
            previewBox->Update(0);
            if (!previewBox->isTool) mapState[y][x] = TILE_OCCUPIED;

            // Remove Preview.
            previewBox = nullptr;
            OnMouseMove(mx, my);
        }
    }
}
