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

    if(MapId==4) DrawCustomize();
    else DrawPreviewMap();
    
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
    if      (keyCode == ALLEGRO_KEY_ESCAPE) BackOnClick(0);
    else if (keyCode == ALLEGRO_KEY_PAD_ENTER || keyCode == ALLEGRO_KEY_ENTER) PlayOnClick(1);
}
void MapSelectScene::PlayOnClick(int id) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = this->MapId ;
    scene->mapCombineW=customizedW;
    scene->mapCombineH=customizedH; // Set the map combine size, can be changed later.
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void MapSelectScene::DrawPreviewMap() {
    const float previewBlockSize = PlayScene::BlockSize*0.45f; 
    
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::ifstream fin(filename);
    std::vector<std::string> lines;
    std::string line;
        
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        lines.push_back(line);
    }
    fin.close();
        
    if (lines.empty()) return;
        
    int mapHeight = lines.size();
    int mapWidth = lines[0].size();
        
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;
    int previewX = halfW - (mapWidth * previewBlockSize) / 2;
    int previewY = halfH - 250;
        
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            Engine::Image* tileImage;
            if (lines[i][j] == '0') {
                tileImage = new Engine::Image("play/grass2.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            } else if (lines[i][j] == '1') {
                tileImage = new Engine::Image("play/rock.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            }else if (lines[i][j] == '2') {
                tileImage = new Engine::Image("play/rock_grass.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            }else if (lines[i][j] == '3') {
                tileImage = new Engine::Image("play/flowers3.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            }
            AddNewObject(tileImage);
        }
    }
        
    AddNewObject(new Engine::Label(std::string("Map ") + std::to_string(MapId), 
        "pirulen.ttf", 48, halfW, previewY - 50, 255, 255, 255, 255, 0.5, 0.5));
    
}
void MapSelectScene::DrawCustomize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Label("Customize", "pirulen.ttf", 60, halfW, halfH - 350, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Width :", "pirulen.ttf", 48, halfW-300, halfH - 150, 255, 255, 255, 255, 1, 0.5));
    AddNewObject(new Engine::Label("maps", "pirulen.ttf", 48, halfW+400, halfH - 150, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Height :", "pirulen.ttf", 48, halfW-300, halfH , 255, 255, 255, 255, 1, 0.5));
    AddNewObject(new Engine::Label("maps", "pirulen.ttf", 48, halfW+400, halfH , 255, 255, 255, 255, 0.5, 0.5));

    WidthLabel= new  Engine::Label(std::to_string(customizedW), "pirulen.ttf", 48, halfW, halfH - 150, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(WidthLabel);
    HeightLabel = new Engine::Label(std::to_string(customizedH) , "pirulen.ttf", 48, halfW, halfH , 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(HeightLabel);

    Engine::ImageButton *btn;   
    //WIDTH
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW +225, halfH -175, 50, 50);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::WidthPlusOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 40, halfW + 250 , halfH -150 , 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 260, halfH -175, 50, 50);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::WidthMinusOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 40, halfW - 235 , halfH - 150,  0, 0, 0, 255, 0.5, 0.5));

    //HEIGHT
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW +225, halfH-25, 50, 50);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::HeigthPlusOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("+", "pirulen.ttf", 40, halfW + 250 , halfH,  0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW -260, halfH-25 , 50, 50);
    btn->SetOnClickCallback(std::bind(&MapSelectScene::HeigthMinusOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("-", "pirulen.ttf", 40, halfW - 235, halfH, 0, 0, 0, 255, 0.5, 0.5));
}
void MapSelectScene::WidthPlusOnClick(int stage){
    if(customizedW < 10) {
        customizedW++;
        WidthLabel->Text = std::to_string(customizedW);
    }
}
void MapSelectScene::WidthMinusOnClick(int stage){
    if(customizedW > 1) {
        customizedW--;
        WidthLabel->Text = std::to_string(customizedW);
    }
}
void MapSelectScene::HeigthPlusOnClick(int stage){
    if(customizedH < 10) {
        customizedH++;
        HeightLabel->Text = std::to_string(customizedH);
    }
}
void MapSelectScene::HeigthMinusOnClick(int stage){
    if(customizedH > 1) {
        customizedH--;
        HeightLabel->Text = std::to_string(customizedH);
    }
}