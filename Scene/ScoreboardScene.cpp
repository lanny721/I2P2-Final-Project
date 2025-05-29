#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

using namespace std;

void ScoreboardScene::Initialize() {
    w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    for (int i = 0; i < 20; i++) co.push_back({cUp, 0, cLow - i * 10});
    UIscoreboardTitle = new Engine::Label("Scoreboard", "pirulen.ttf", 64, w / 2, 50, 255, 255, 255, 255, 0.5, 0.5);
    UIrank = new Engine::Label("Rank", "pirulen.ttf", 44, w / 2 - 480, 130, 255, 255, 255, 255, 0.5, 0.5);
    UIname = new Engine::Label("Player", "pirulen.ttf", 44, w / 2 - 100, 130, 255, 255, 255, 255, 0.5, 0.5);
    UIscore = new Engine::Label("Score", "pirulen.ttf", 44, w / 2 + 240, 130, 255, 255, 255, 255, 0.5, 0.5);
    UItime = new Engine::Label("Time", "pirulen.ttf", 44, w / 2 + 500, 130, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(UIscoreboardTitle);
    AddNewObject(UIrank);
    AddNewObject(UIname);
    AddNewObject(UIscore);
    AddNewObject(UItime);

    Engine::ImageButton *btn;

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w/2 - 200, 700, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, w/2, 750, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w/2 - 700, 700, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PrevOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 36, w/2 - 700 + 200, 750, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w/2 + 300, 700, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::NextOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 36, w/2 + 300 + 200, 750, 0, 0, 0, 255, 0.5, 0.5));

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    
    std::string line;
    PlayerData tmpData;
    ifstream scoreFile("../Resource/scoreboard.txt");
    while (std::getline(scoreFile, line)) {
        std::stringstream ss(line);
        if (ss >> tmpData.name >> tmpData.score) {
            ss >> tmpData.timeinfo;
            playerDataList.push_back(tmpData);
        }
    }
    sort(playerDataList.begin(), playerDataList.end(), [](const PlayerData& a, const PlayerData& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.timeinfo < b.timeinfo; // Sort by time if scores are equal
    });

    ToPage(1);
}
void ScoreboardScene::Terminate() {
    playerDataList.clear();
    co.clear();
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void ScoreboardScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        Engine::GameEngine::GetInstance().ChangeScene("start");
        return;
    }
}
void ScoreboardScene::PrevOnClick() {
    if (page > 1) {
        removePlayerLabels();
        ToPage(page - 1);
    }
}
void ScoreboardScene::NextOnClick() {
    if (page < (playerDataList.size() - 1) / 10 + 1) {
        removePlayerLabels();
        ToPage(page + 1);
    }
}
void ScoreboardScene::removePlayerLabels() {
    for(size_t i = 0 ; i < min<size_t>(playerDataList.size()-10*(page-1), 10); ++i) {
        RemoveObject(playerDataList[i+10*(page-1)].rankLabel->GetObjectIterator());
        RemoveObject(playerDataList[i+10*(page-1)].nameLabel->GetObjectIterator());
        RemoveObject(playerDataList[i+10*(page-1)].scoreLabel->GetObjectIterator());
        playerDataList[i+10*(page-1)].rankLabel = nullptr;
        playerDataList[i+10*(page-1)].nameLabel = nullptr;
        playerDataList[i+10*(page-1)].scoreLabel = nullptr;

        RemoveObject(playerDataList[i+10*(page-1)].timeLabel->GetObjectIterator());
        playerDataList[i+10*(page-1)].timeLabel = nullptr;
    }
}
void ScoreboardScene::ToPage(int page) {
    this->page = page;
    for (size_t i = 0 ; i < min<size_t>(playerDataList.size()-10*(page-1), 10); ++i) {
        playerDataList[i+10*(page-1)].rankLabel = new Engine::Label(to_string(i+1+10*(page-1)), "pirulen.ttf", 36, w/2-460, 200 + i * 48, 255, 255, 255, 255, 1, 0.5);
        playerDataList[i+10*(page-1)].nameLabel = new Engine::Label(playerDataList[i+10*(page-1)].name, "pirulen.ttf", 36, w/2-350, 200 + i * 48, 255, 255, 255, 255, 0, 0.5);
        playerDataList[i+10*(page-1)].scoreLabel = new Engine::Label(to_string(playerDataList[i+10*(page-1)].score), "pirulen.ttf", 36, w/2+330, 200 + i * 48, 255, 255, 255, 255, 1, 0.5);
        AddNewObject(playerDataList[i+10*(page-1)].rankLabel);
        AddNewObject(playerDataList[i+10*(page-1)].nameLabel);
        AddNewObject(playerDataList[i+10*(page-1)].scoreLabel);
        
        playerDataList[i+10*(page-1)].timeLabel = new Engine::Label(playerDataList[i+10*(page-1)].timeinfo, "pirulen.ttf", 24, w/2+400, 200 + i * 48, 255, 255, 255, 255, 0, 0.5);
        AddNewObject(playerDataList[i+10*(page-1)].timeLabel);
    }
    updateColor();
}
void ScoreboardScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 0.05f) {
        ticks -= 0.05f;
        updateColor();
    }   
}
void ScoreboardScene::updateColor() {
    for (size_t i = 0; i < co.size(); i++) {
        if      (co[i].r >= cUp  && co[i].g <  cUp  && co[i].b <= cLow) co[i].g += 5;
        else if (co[i].r >  cLow && co[i].g >= cUp  && co[i].b <= cLow) co[i].r -= 5;
        else if (co[i].r <= cLow && co[i].g >= cUp  && co[i].b <  cUp ) co[i].b += 5;
        else if (co[i].r <= cLow && co[i].g >  cLow && co[i].b >= cUp ) co[i].g -= 5;
        else if (co[i].r <  cUp  && co[i].g <= cLow && co[i].b >= cUp ) co[i].r += 5;
        else if (co[i].r >= cUp  && co[i].g <= cLow && co[i].b >  cLow) co[i].b -= 5;
        else std::cout << "not chang" << ' ' << co[i].r << ' ' << co[i].g << ' ' << co[i].b << std::endl;
        //std::cout << "Color " << i << ": (" << co[i].r << ", " << co[i].g << ", " << co[i].b << ")" << std::endl;
    }

    UIscoreboardTitle->Color = al_map_rgb(co[defaultColorIdx].r, co[defaultColorIdx].g, co[defaultColorIdx].b);
    UIrank ->Color           = al_map_rgb(co[defaultColorIdx].r, co[defaultColorIdx].g, co[defaultColorIdx].b);
    UIname ->Color           = al_map_rgb(co[defaultColorIdx].r, co[defaultColorIdx].g, co[defaultColorIdx].b);
    UIscore->Color           = al_map_rgb(co[defaultColorIdx].r, co[defaultColorIdx].g, co[defaultColorIdx].b);
    UItime ->Color           = al_map_rgb(co[defaultColorIdx].r, co[defaultColorIdx].g, co[defaultColorIdx].b);
    for (size_t i = 0 ; i < min<size_t>(playerDataList.size()-10*(page-1), 10); ++i) {
        playerDataList[i+10*(page-1)].rankLabel->Color = al_map_rgb(co[i + 0].r, co[i + 0].g, co[i + 0].b);
        playerDataList[i+10*(page-1)].nameLabel->Color = al_map_rgb(co[i + 1].r, co[i + 1].g, co[i + 1].b);
        playerDataList[i+10*(page-1)].scoreLabel->Color= al_map_rgb(co[i + 2].r, co[i + 2].g, co[i + 2].b);
        playerDataList[i+10*(page-1)].timeLabel->Color = al_map_rgb(co[i + 3].r, co[i + 3].g, co[i + 3].b);
    }
}
