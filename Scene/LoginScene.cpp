#include "LoginScene.hpp"
#include <allegro5/allegro.h>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void LoginScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // 添加背景
    AddNewObject(new Engine::Image("play/start_background.png", 0, 0, w, h));

    // 添加標題
    AddNewObject(new Engine::Label("Login", "pirulen.ttf", 80, halfW, halfH / 3, 255, 255, 255, 255, 0.5, 0.5));

    // 提示用戶在終端輸入
    AddNewObject(new Engine::Label("Enter username and password in terminal", "pirulen.ttf", 40, halfW, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));

    // 提交按鈕
    Engine::ImageButton* btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&LoginScene::SubmitOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    // 返回按鈕
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&LoginScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // 播放背景音樂
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void LoginScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void LoginScene::SubmitOnClick() {
    std::string username, password;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    // 檢查輸入是否為空
    if (username.empty() || password.empty()) {
        std::cout << "Username or password cannot be empty!\n";
        AddNewObject(new Engine::Label("Empty Input", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 300, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }

    // 檢查帳號和密碼
    std::ifstream file("users.txt");
    if (!file.is_open()) {
        std::cout << "Cannot open users.txt for reading!\n";
        AddNewObject(new Engine::Label("File Error", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 300, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }

    std::string line;
    bool loginSuccess = false;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string stored_username = line.substr(0, pos);
            std::string stored_password = line.substr(pos + 1);
            if (stored_username == username && stored_password == password) {
                loginSuccess = true;
                break;
            }
        }
    }
    file.close();

    if (loginSuccess) {
        std::cout << "Login successful!\n";
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    } else {
        std::cout << "Login failed!\n";
        AddNewObject(new Engine::Label("Login Failed", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 300, 255, 0, 0, 255, 0.5, 0.5));
    }
}

void LoginScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void LoginScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ENTER) {
        SubmitOnClick();
    } else if (keyCode == ALLEGRO_KEY_ESCAPE) {
        BackOnClick(1);
    }
}