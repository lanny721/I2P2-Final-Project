#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class StageSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int totalStages = 4; // Total number of stages available

public:
    explicit StageSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
};

#endif   // STAGESELECTSCENE_HPP
