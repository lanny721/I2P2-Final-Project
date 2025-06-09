#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
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

    //zzy
    totalPage = (playerDataList.size() <= 10) ? 1 : (playerDataList.size() - 1) / 10 + 1;
    std::string pageText = "(" + std::to_string(page) + "/" + std::to_string(totalPage) + ")";
    pageLabel = new Engine::Label(pageText, "pirulen.ttf", 45, w/2 + 450, 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(pageLabel);
    //

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

    //zzy
    if (keyCode == ALLEGRO_KEY_BACKSPACE && selectedIndex >= 0) {
        // Remove the selected record
        playerDataList.erase(playerDataList.begin() + selectedIndex);
        
        // Save to file
        std::ofstream scoreFile("../Resource/scoreboard.txt", std::ios::trunc);
        for (const auto& data : playerDataList) {
            scoreFile << data.name << " " << data.score << " " << data.timeinfo << "\n";
        }
        scoreFile.close();

        // if (page > 1 && playerDataList.size() <= (page-1) * 10) {
        //     page--;
        // }
        // Update page count
        totalPage = (playerDataList.size() <= 10) ? 1 : (playerDataList.size() - 1) / 10 + 1;
        //if (page > totalPage) page = totalPage;
        if (playerDataList.empty()) {
            page = 1;
        } else if (page > totalPage) {
            page = totalPage;
        } else if (page > 1 && (page-1) * 10 >= playerDataList.size()) {
            page--;
        }
        
        // Refresh display
        removePlayerLabels();
        if (pageLabel) {
            RemoveObject(pageLabel->GetObjectIterator());
            pageLabel = nullptr;
        }
        selectedIndex = -1;
        ToPage(page);
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
//xiiigua origin
// void ScoreboardScene::removePlayerLabels() {
//     for(size_t i = 0 ; i < min<size_t>(playerDataList.size()-10*(page-1), 10); ++i) {
//         RemoveObject(playerDataList[i+10*(page-1)].rankLabel->GetObjectIterator());
//         RemoveObject(playerDataList[i+10*(page-1)].nameLabel->GetObjectIterator());
//         RemoveObject(playerDataList[i+10*(page-1)].scoreLabel->GetObjectIterator());
//         playerDataList[i+10*(page-1)].rankLabel = nullptr;
//         playerDataList[i+10*(page-1)].nameLabel = nullptr;
//         playerDataList[i+10*(page-1)].scoreLabel = nullptr;

//         RemoveObject(playerDataList[i+10*(page-1)].timeLabel->GetObjectIterator());
//         playerDataList[i+10*(page-1)].timeLabel = nullptr;
//     }
// }
void ScoreboardScene::removePlayerLabels() {
    size_t startIdx = (page - 1) * 10;
    size_t endIdx = min(startIdx + 10, playerDataList.size());
    
    for(size_t i = startIdx; i < endIdx; ++i) {
        if (playerDataList[i].rankLabel) {
            RemoveObject(playerDataList[i].rankLabel->GetObjectIterator());
            playerDataList[i].rankLabel = nullptr;
        }
        if (playerDataList[i].nameLabel) {
            RemoveObject(playerDataList[i].nameLabel->GetObjectIterator());
            playerDataList[i].nameLabel = nullptr;
        }
        if (playerDataList[i].scoreLabel) {
            RemoveObject(playerDataList[i].scoreLabel->GetObjectIterator());
            playerDataList[i].scoreLabel = nullptr;
        }
        if (playerDataList[i].timeLabel) {
            RemoveObject(playerDataList[i].timeLabel->GetObjectIterator());
            playerDataList[i].timeLabel = nullptr;
        }
    }
}
//xiiigua origin
// void ScoreboardScene::ToPage(int page) {
//     this->page = page;
//     for (size_t i = 0 ; i < min<size_t>(playerDataList.size()-10*(page-1), 10); ++i) {
//         playerDataList[i+10*(page-1)].rankLabel = new Engine::Label(to_string(i+1+10*(page-1)), "pirulen.ttf", 36, w/2-460, 200 + i * 48, 255, 255, 255, 255, 1, 0.5);
//         playerDataList[i+10*(page-1)].nameLabel = new Engine::Label(playerDataList[i+10*(page-1)].name, "pirulen.ttf", 36, w/2-350, 200 + i * 48, 255, 255, 255, 255, 0, 0.5);
//         playerDataList[i+10*(page-1)].scoreLabel = new Engine::Label(to_string(playerDataList[i+10*(page-1)].score), "pirulen.ttf", 36, w/2+330, 200 + i * 48, 255, 255, 255, 255, 1, 0.5);
//         AddNewObject(playerDataList[i+10*(page-1)].rankLabel);
//         AddNewObject(playerDataList[i+10*(page-1)].nameLabel);
//         AddNewObject(playerDataList[i+10*(page-1)].scoreLabel);
        
//         playerDataList[i+10*(page-1)].timeLabel = new Engine::Label(playerDataList[i+10*(page-1)].timeinfo, "pirulen.ttf", 24, w/2+400, 200 + i * 48, 255, 255, 255, 255, 0, 0.5);
//         AddNewObject(playerDataList[i+10*(page-1)].timeLabel);
//     }
//     //zzy
//     if (pageLabel) {
//         RemoveObject(pageLabel->GetObjectIterator());
//     }
//     std::string pageText = "(" + std::to_string(page) + "/" + std::to_string(totalPage) + ")";
//     pageLabel = new Engine::Label(pageText, "pirulen.ttf", 45, w/2 + 325, 50, 255, 255, 255, 255, 0.5, 0.5);
//     AddNewObject(pageLabel);
//     //
    
//     updateColor();
// }
//zzy
void ScoreboardScene::ToPage(int newPage) {
    // Update page number
    this->page = newPage;
    
    // Clear all existing labels first
    removePlayerLabels();
    
    // Calculate valid range for current page
    size_t startIdx = (page - 1) * 10;
    size_t itemsToShow = min<size_t>(playerDataList.size() - startIdx, 10);
    
    // Create new labels
    for (size_t i = 0; i < itemsToShow; ++i) {
        size_t dataIdx = startIdx + i;
        int yPos = 200 + i * 48;
        
        // Create and add new labels
        playerDataList[dataIdx].rankLabel = new Engine::Label(
            to_string(dataIdx + 1), "pirulen.ttf", 36, 
            w/2-460, yPos, 255, 255, 255, 255, 1, 0.5);
        playerDataList[dataIdx].nameLabel = new Engine::Label(
            playerDataList[dataIdx].name, "pirulen.ttf", 36, 
            w/2-350, yPos, 255, 255, 255, 255, 0, 0.5);
        playerDataList[dataIdx].scoreLabel = new Engine::Label(
            to_string(playerDataList[dataIdx].score), "pirulen.ttf", 36, 
            w/2+330, yPos, 255, 255, 255, 255, 1, 0.5);
        playerDataList[dataIdx].timeLabel = new Engine::Label(
            playerDataList[dataIdx].timeinfo, "pirulen.ttf", 24, 
            w/2+400, yPos, 255, 255, 255, 255, 0, 0.5);
                       
        AddNewObject(playerDataList[dataIdx].rankLabel);
        AddNewObject(playerDataList[dataIdx].nameLabel);
        AddNewObject(playerDataList[dataIdx].scoreLabel);
        AddNewObject(playerDataList[dataIdx].timeLabel);
    }
     
    // Update page label
    if (pageLabel) {
        RemoveObject(pageLabel->GetObjectIterator());
    }
    std::string pageText = "(" + std::to_string(page) + "/" + std::to_string(totalPage) + ")";
    pageLabel = new Engine::Label(pageText, "pirulen.ttf", 45, w/2 + 450, 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(pageLabel);
    
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

void ScoreboardScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    
    // Check if click is in the scoreboard area
    int clickY = my - 200;
    int recordIndex = clickY / 48;
    int startIndex = (page - 1) * 10;

    if (recordIndex >= 0 && recordIndex < 10 && 
        (startIndex + recordIndex) < playerDataList.size()) {
        
        if (selectedIndex == startIndex + recordIndex) {
            selectedIndex = -1;  // Deselect
        } else {
            selectedIndex = startIndex + recordIndex;
            selectionBoxY = 200 + 48 * recordIndex;
            selectionBoxX = w/2;
        }
    }
}
void ScoreboardScene::Draw() const {
    IScene::Draw();
    if (selectedIndex >= 0) {
        al_draw_filled_rectangle(
            w/2 - 750,        // x1
            selectionBoxY - 24,         // y1
            w/2 + 750,        // x2
            selectionBoxY + 24,         // y2
            al_map_rgba(0, 0, 255, 64)  // Semi-transparent blue
        );
    }
}