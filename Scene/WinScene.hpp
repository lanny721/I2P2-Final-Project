#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include "UI/Component/Label.hpp"

class WinScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    void SaveScore();

public:
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void LoginOnClick(int stage); // 新增
    void RegisterOnClick(int stage); // 新增
    void OnKeyDown(int keyCode) override;
};

#endif   // WINSCENE_HPP
