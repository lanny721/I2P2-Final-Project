#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <random>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "MapSelectScene.hpp"
#include "StageSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void MapSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;    
    
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW*3/2 - 200, halfH / 2 + 400, 400, 100);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW*3/2, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2 - 200, halfH / 2 + 400, 400, 100);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::BackOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW/2, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    
}
void MapSelectScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void MapSelectScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void MapSelectScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
}
void MapSelectScene::PlayOnClick(int id) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = this->MapId ;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}