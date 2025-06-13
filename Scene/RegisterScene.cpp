#include "RegisterScene.hpp"
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

void RegisterScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // 初始化狀態
    usernameEntered = false;
    passwordEntered = false;
    username.clear();
    password.clear();

    // 添加背景
    AddNewObject(new Engine::Image("play/start_background.png", 0, 0, w, h));

    // 添加標題
    AddNewObject(new Engine::Label("Register", "pirulen.ttf", 80, halfW, halfH / 3, 255, 255, 255, 255, 0.5, 0.5));

    // 提示當前輸入
    AddNewObject(new Engine::Label("press enter to submit", "pirulen.ttf", 40, halfW, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));

    // 帳號顯示
    UIUsername = new Engine::Label("account: " + username, "pirulen.ttf", 32, halfW, halfH -100, 255, 255, 100, 255, 0.5, 0.5);
    AddNewObject(UIUsername);

    // 密碼顯示（初始為空）
    UIPassword = new Engine::Label("Password: ", "pirulen.ttf", 32, halfW, halfH , 255, 255, 100, 255, 0.5, 0.5);
    AddNewObject(UIPassword);

    // 返回按鈕
    Engine::ImageButton* btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&RegisterScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    // 播放背景音樂
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void RegisterScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void RegisterScene::SubmitOnClick() {
    if (!usernameEntered || !passwordEntered) {
        std::cout << "Please enter both account and password!\n";
        AddNewObject(new Engine::Label("Complete Input", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }

    // 檢查輸入是否為空
    if (username.empty() || password.empty()) {
        std::cout << "account or password cannot be empty!\n";
        AddNewObject(new Engine::Label("Empty Input", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }

    // 檢查帳號是否已存在
    std::ifstream file("../Resource/users.txt");
    std::string line;
    bool usernameTaken = false;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string stored_username = line.substr(0, pos);
            if (stored_username == username) {
                usernameTaken = true;
                break;
            }
        }
    }
    file.close();

    if (usernameTaken) {
        std::cout << "account already exists!\n";
        AddNewObject(new Engine::Label("account Exists", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }

    // 寫入 users.txt
    std::ofstream outFile("../Resource/users.txt", std::ios::app);
    if (!outFile.is_open()) {
        std::cout << "Cannot open users.txt for writing!\n";
        AddNewObject(new Engine::Label("File Error", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
        return;
    }
    outFile << username << ":" << password << "\n";
    outFile.close();

    std::cout << "Registration successful!\n";
    UIUsername->Color = al_map_rgba(255, 255, 255, 255);
    UIPassword->Color = al_map_rgba(255, 255, 255, 255);
    Engine::GameEngine::GetInstance().SetCurrentUsername(username);
    Engine::GameEngine::GetInstance().ChangeScene("win");
}

void RegisterScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("win");
}

void RegisterScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        BackOnClick(1);
        return;
    }

    if (passwordEntered) return; // 帳號和密碼都輸入完成後不再接受輸入

    if (keyCode == ALLEGRO_KEY_ENTER) {
        if (!usernameEntered) {
            if (username.empty()) {
                std::cout << "account cannot be empty!\n";
                AddNewObject(new Engine::Label("Empty account", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
                return;
            }
            usernameEntered = true;
            UIUsername->Color = al_map_rgba(255, 255, 255, 255); // 確認帳號後變白色
            std::cout << "account entered: " << username << ". Now enter password.\n";
        } else {
            if (password.empty()) {
                std::cout << "Password cannot be empty!\n";
                AddNewObject(new Engine::Label("Empty Password", "pirulen.ttf", 40, Engine::GameEngine::GetInstance().GetScreenSize().x / 2, Engine::GameEngine::GetInstance().GetScreenSize().y / 2 + 350, 255, 0, 0, 255, 0.5, 0.5));
                return;
            }
            passwordEntered = true;
            SubmitOnClick(); // 自動提交
        }
        return;
    }

    if (!usernameEntered) {
        // 輸入帳號
        if (username.size() < 15 && keyCode != ALLEGRO_KEY_BACKSPACE && al_keycode_to_name(keyCode)[0] != '\0') {
            std::string key = al_keycode_to_name(keyCode);
            if (key.length() == 1 && ((key[0] >= 'A' && key[0] <= 'Z') || (key[0] >= 'a' && key[0] <= 'z'))) {
                username += key;
                UIUsername->Text = "account: " + username;
                std::cout << "Current account: " << username << std::endl;
            } else {
                std::cout << "Only letters (a-z, A-Z) allowed for account!\n";
            }
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE && !username.empty()) {
            username.pop_back();
            UIUsername->Text = "account: " + username;
            std::cout << "Current account: " << username << std::endl;
        }
    } else {
        // 輸入密碼
        if (password.size() < 15 && keyCode != ALLEGRO_KEY_BACKSPACE && al_keycode_to_name(keyCode)[0] != '\0') {
            password += std::string(al_keycode_to_name(keyCode));
            std::string displayPassword(password.size(), '*'); // 顯示星號
            UIPassword->Text = "Password: " + displayPassword;
            std::cout << "Current password: [hidden]\n";
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE && !password.empty()) {
            password.pop_back();
            std::string displayPassword(password.size(), '*');
            UIPassword->Text = "Password: " + displayPassword;
            std::cout << "Current password: [hidden]\n";
        }
    }
}