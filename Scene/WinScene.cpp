#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    //name.clear();

    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH - 50, 415, 415, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("chunyao.ogg");

    //login button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2- 190, halfH / 2 +200, 380, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::LoginOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Login", "pirulen.ttf", 48, halfW/2, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));
    //register button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2*3 - 190, halfH / 2 + 200, 390, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::RegisterOnClick, this, 5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Register", "pirulen.ttf", 48, halfW/2*3, halfH / 2 +250, 0, 0, 0, 255, 0.5, 0.5));
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
            ticks = 100;
            bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::BackOnClick(int stage) {
    SaveScore();
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}
void WinScene::LoginOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("login");
}

void WinScene::RegisterOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("register");
}
void WinScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        Engine::GameEngine::GetInstance().ChangeScene("start");
        return;
    }
}
void WinScene::SaveScore() {
        std::string username = Engine::GameEngine::GetInstance().GetCurrentUsername();
        std::cout << "Saving score for username: " << username << std::endl;

        time_t now = time(0);
        std::tm *lctm = std::localtime(&now);

        std::ofstream ofs("../Resource/scoreboard.txt", std::ios::app);
        ofs << username << ' ';
        ofs << dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->GetMoney() << ' ';
        ofs << std::setw(2) << std::setfill('0') << lctm->tm_mon + 1 << '/';
        ofs << std::setw(2) << std::setfill('0') << lctm->tm_mday << '_';
        ofs << std::setw(2) << std::setfill('0') << lctm->tm_hour << ':';
        ofs << std::setw(2) << std::setfill('0') << lctm->tm_min << ':';
        ofs << std::setw(2) << std::setfill('0') << lctm->tm_sec << std::endl;
        ofs.close();

        
        std::cout << "Name saved to scoreboard." << std::endl;
}
