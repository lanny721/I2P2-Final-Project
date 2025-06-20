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
#include <iostream>
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

    customizedW=3;
    customizedH=4;
    buttonCoolDown = 0.15f;
    customizedEnemy=13;

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
    if(MapId != 3) {
        PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
        scene->MapId = this->MapId ;
        scene->mapCombineW=customizedW;
        scene->mapCombineH=customizedH;
        scene->enemyWaveNum = customizedEnemy;
        Engine::GameEngine::GetInstance().ChangeScene("play");
    } else Engine::GameEngine::GetInstance().ChangeScene("sand-box");
}
void MapSelectScene::DrawPreviewMap() {
    const float previewBlockSize = PlayScene::BlockSize*0.45f; 
    
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::ifstream fin(filename);
    std::vector<std::string> lines;
    std::string line;
        
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        if(MapId==3) lines.push_back("00000000000000000000");
        else lines.push_back(line);
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
            }else if(lines[i][j] == '4') {
                tileImage = new Engine::Image("play/water.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            }else if(lines[i][j] == '5') {
                tileImage = new Engine::Image("play/gold.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            } else if (lines[i][j] == '6') {
                tileImage = new Engine::Image("play/door.png", 
                    previewX + j * previewBlockSize, 
                    previewY + i * previewBlockSize, 
                    previewBlockSize, previewBlockSize);
            } else continue;
            AddNewObject(tileImage);
        }
    }

    if(MapId==3){
        AddNewObject(new Engine::Label(std::string("Create your"), "pirulen.ttf", 52, halfW, previewY + mapHeight*previewBlockSize/2 -30 , 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(std::string("own map!"), "pirulen.ttf", 52, halfW, previewY +mapHeight*previewBlockSize/2 + 30 , 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(std::string("sand box mode"), 
            "pirulen.ttf", 48, halfW, previewY - 60, 255, 255, 255, 255, 0.5, 0.5));
    }
    else {
        AddNewObject(new Engine::Label(std::string("Map ") + std::to_string(MapId), 
            "pirulen.ttf", 48, halfW, previewY - 50, 255, 255, 255, 255, 0.5, 0.5));
    }
}
void MapSelectScene::DrawCustomize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Label("Customize", "pirulen.ttf", 60, halfW, halfH - 350, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Width :", "pirulen.ttf", 48, halfW-300, halfH - 150, 255, 255, 255, 255, 1, 0.5));
    AddNewObject(new Engine::Label("maps", "pirulen.ttf", 48, halfW+320, halfH - 150, 255, 255, 255, 255, 0, 0.5));
    AddNewObject(new Engine::Label("Height :", "pirulen.ttf", 48, halfW-300, halfH - 50, 255, 255, 255, 255, 1, 0.5));
    AddNewObject(new Engine::Label("maps", "pirulen.ttf", 48, halfW+320, halfH - 50, 255, 255, 255, 255, 0, 0.5));
    AddNewObject(new Engine::Label("Enemy :", "pirulen.ttf", 48, halfW-300, halfH + 50, 255, 255, 255, 255, 1, 0.5));
    AddNewObject(new Engine::Label("waves", "pirulen.ttf", 48, halfW+320, halfH + 50, 255, 255, 255, 255, 0, 0.5));

    WidthLabel= new  Engine::Label(std::to_string(customizedW), "pirulen.ttf", 48, halfW, halfH - 150, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(WidthLabel);
    HeightLabel = new Engine::Label(std::to_string(customizedH) , "pirulen.ttf", 48, halfW, halfH - 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(HeightLabel);
    EnemyLabel = new Engine::Label(std::to_string(customizedEnemy), "pirulen.ttf", 48, halfW, halfH + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(EnemyLabel);

    Engine::ImageButton *btn;   
    //WIDTH
    //+
    WidthPlusButton = new Engine::ImageButton("stage-select/button_add.png", "stage-select/button_add_pressed.png", halfW +200, halfH -185, 68, 68);
    WidthPlusButton->SetOnClickCallback(std::bind(&MapSelectScene::WidthPlusOnClick, this, 0));
    AddNewControlObject(WidthPlusButton);

    //-
    WidthMinusButton = new Engine::ImageButton("stage-select/button_minus.png", "stage-select/button_minus_pressed.png", halfW - 245, halfH -185, 68, 68);
    WidthMinusButton->SetOnClickCallback(std::bind(&MapSelectScene::WidthMinusOnClick, this, 1));
    AddNewControlObject(WidthMinusButton);

    //HEIGHT
    //+
    HeightPlusButton = new Engine::ImageButton("stage-select/button_add.png", "stage-select/button_add_pressed.png", halfW +200, halfH-85, 68, 68);
    HeightPlusButton->SetOnClickCallback(std::bind(&MapSelectScene::HeigthPlusOnClick, this, 2));
    AddNewControlObject(HeightPlusButton);

    //-
    HeightMinusButton = new Engine::ImageButton("stage-select/button_minus.png", "stage-select/button_minus_pressed.png", halfW -245, halfH-85 , 68, 68);
    HeightMinusButton->SetOnClickCallback(std::bind(&MapSelectScene::HeigthMinusOnClick, this, 3));
    AddNewControlObject(HeightMinusButton);

    //enemy waves
    //+
    EnemyPlusButton = new Engine::ImageButton("stage-select/button_add.png", "stage-select/button_add_pressed.png", halfW +200, halfH+15, 68, 68);
    EnemyPlusButton->SetOnClickCallback(std::bind(&MapSelectScene::EnemyPlusOnClick, this, 4));
    AddNewControlObject(EnemyPlusButton);

    //-
    EnemyMinusButton = new Engine::ImageButton("stage-select/button_minus.png", "stage-select/button_minus_pressed.png", halfW -245, halfH+15 , 68, 68);
    EnemyMinusButton->SetOnClickCallback(std::bind(&MapSelectScene::EnemyMinusOnClick, this, 5));
    AddNewControlObject(EnemyMinusButton);
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
void MapSelectScene::EnemyPlusOnClick(int stage){
    if(customizedEnemy < 25) {
        customizedEnemy++;
        EnemyLabel->Text = std::to_string(customizedEnemy);
    }
}
void MapSelectScene::EnemyMinusOnClick(int stage){
    if(customizedEnemy > 1) {
        customizedEnemy--;
        EnemyLabel->Text = std::to_string(customizedEnemy);
    }
}
// void MapSelectScene::Update(float deltaTime) {
//     buttonCoolDown -= deltaTime;
//     if(buttonCoolDown < 0) buttonCoolDown = 0;

//     //bool isDown = Engine::GameEngine::GetInstance().isMouseDown;
//     if(buttonCoolDown == 0){
//         if(Engine::GameEngine::GetInstance().isMouseDown) {
//             if(EnemyPlusButton->IsMouseIn()) {
//                 EnemyPlusOnClick(4);
//             } else if(EnemyMinusButton->IsMouseIn()) {
//                 EnemyMinusOnClick(5);
//             } else if(WidthPlusButton->IsMouseIn()) {
//                 WidthPlusOnClick(0);
//             } else if(WidthMinusButton->IsMouseIn()) {
//                 WidthMinusOnClick(1);
//             } else if(HeightPlusButton->IsMouseIn()) {
//                 HeigthPlusOnClick(2);
//             } else if(HeightMinusButton->IsMouseIn()) {
//                 HeigthMinusOnClick(3);
//             }
//             buttonCoolDown = 0.15f;
//         }
//     }
// }