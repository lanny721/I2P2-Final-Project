#ifndef SANDBOXSCENE_HPP
#define SANDBOXSCENE_HPP
#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
#include "Scene/PlayScene.hpp"
#include "turret/SandBox.hpp"
#include <vector>
class PlayScene;

class SandBoxScene final : public Engine::IScene {
private:
    int MapWidth = PlayScene::defW;
    int MapHeight = PlayScene::defH;
    int uiBoundaryX = 1280;
    Group *TileMapGroup;
    Group *UIGroup;
    std::vector<std::vector<PlayScene::TileType>> mapState; // [y][x] important!!!!!!!!!!!!!! 
    std::vector<std::vector<Engine::Image*>> TileMapImages; // [y][x]
    void ConstructUI();
    void UIBtnClicked(int id);
    void SaveOnClick(int stage);
    void putThings(int button, int mx, int my);
    Turret*previewBox = nullptr;

public:
    explicit SandBoxScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
};

#endif