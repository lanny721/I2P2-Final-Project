#ifndef LOGIN_SCENE_HPP
#define LOGIN_SCENE_HPP

#include <memory>
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class LoginScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
    explicit LoginScene() = default;
    void Initialize() override;
    void Terminate() override;
    void SubmitOnClick();
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
};

#endif // LOGIN_SCENE_HPP