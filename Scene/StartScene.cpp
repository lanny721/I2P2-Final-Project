//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    Engine::Image* background = new Engine::Image("play/start_background.png", // 使用你提供的天空圖或類似圖片
                                                 0, 0,
                                                 w, h);
    background->followCamera = false; // 背景固定在螢幕上
    AddNewObject(background); // 將背景加入場景，最底層渲染

    AddNewObject(new Engine::Label("Castle Defense", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 325, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH / 2 + 375, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 450, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::ScoreboardOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 36, halfW, halfH / 2 + 500, 0, 0, 0, 255, 0.5, 0.5));

    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    //login button
    /*btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2- 190, halfH / 2 +200, 380, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::LoginOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Login", "pirulen.ttf", 48, halfW/2, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));
    //register button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2*3 - 190, halfH / 2 + 200, 390, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::RegisterOnClick, this, 5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Register", "pirulen.ttf", 48, halfW/2*3, halfH / 2 +250, 0, 0, 0, 255, 0.5, 0.5));*/
}
void StartScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ENTER) PlayOnClick(1);
    else if (keyCode == ALLEGRO_KEY_S) SettingsOnClick(2);
    else if (keyCode == ALLEGRO_KEY_L) ScoreboardOnClick(3);
    //else if (keyCode == ALLEGRO_KEY_I) LoginOnClick(4); // 新增快捷鍵 I 給 Login
    //else if (keyCode == ALLEGRO_KEY_R) RegisterOnClick(5); // 新增快捷鍵 R 給 Register
}
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}
void StartScene::ScoreboardOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

