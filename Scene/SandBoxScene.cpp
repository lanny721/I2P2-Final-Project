#include "Scene/SandBoxScene.hpp"
#include "Scene/PlayScene.hpp"

void SandBoxScene::Initialize() {
    AddNewObject(TileMapGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    mapState = std::vector<std::vector<PlayScene::TileType>>(MapHeight, std::vector<PlayScene::TileType>(MapWidth, PlayScene::TileType::TILE_DIRT));
    TileMapImages = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
    for(int i=0;i<MapHeight;i++) {
        for(int j=0;j<MapWidth;j++) {
            TileMapImages[i][j] = new Engine::Image("play/grass2.png", i * PlayScene::BlockSize, j * PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize);
        }
    }
}
void SandBoxScene::Terminate() {

}
void SandBoxScene::Update(float deltaTime) {

}
void SandBoxScene::Draw() const {

}
void SandBoxScene::SaveOnClick(int stage) {

}
void SandBoxScene::OnKeyDown(int keyCode) {

}
void SandBoxScene::OnMouseDown(int button, int mx, int my) {
    
}
void SandBoxScene::OnMouseMove(int mx, int my) {
    
}
void SandBoxScene::OnMouseUp(int button, int mx, int my) {
    
}
