#ifndef LOGIN_SCENE_HPP
#define LOGIN_SCENE_HPP

#include <memory>
#include <string>
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include "UI/Component/Label.hpp"

class LoginScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    bool usernameEntered; // 是否已輸入帳號
    bool passwordEntered; // 是否已輸入密碼
    std::string username; // 當前輸入的帳號
    std::string password; // 當前輸入的密碼
    Engine::Label* UIUsername; // 顯示帳號的標籤
    Engine::Label* UIPassword; // 顯示密碼的標籤
public:
    explicit LoginScene() = default;
    void Initialize() override;
    void Terminate() override;
    void SubmitOnClick();
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
};

#endif // LOGIN_SCENE_HPP