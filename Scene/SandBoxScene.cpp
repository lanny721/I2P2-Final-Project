#include "Scene/SandBoxScene.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"
#include <functional>
#include "UI/Component/Label.hpp"

void SandBoxScene::Initialize() {
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

}
void SandBoxScene::Update(float deltaTime) {

}
void SandBoxScene::Draw() const {
    IScene::Draw();
}
void SandBoxScene::SaveOnClick(int stage) {

}
void SandBoxScene::OnKeyDown(int keyCode) {

}
void SandBoxScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
}
void SandBoxScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void SandBoxScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
}
void SandBoxScene::ConstructUI() {
    // Background
    Engine::Image* boundary=new Engine::Image("play/sand.png", uiBoundaryX, 0, 320, 832);
    UIGroup->AddNewObject(boundary);
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("sandbox"), "pirulen.ttf", 32, uiBoundaryX + 14, 20));

    Engine::ImageButton *btn;
    const int bs=76;// Button size
    // Button 1
    btn = new Engine::ImageButton("play/grass2.png", "play/grass2.png", uiBoundaryX + 14, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new Engine::ImageButton("play/rock.png", "play/rock.png", uiBoundaryX + 14 + bs, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    //Button 3
    btn = new Engine::ImageButton("play/rock_grass.png", "play/rock_grass.png", uiBoundaryX + 14 + bs * 2, 80, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SandBoxScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
}
void SandBoxScene::UIBtnClicked(int id) {

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
