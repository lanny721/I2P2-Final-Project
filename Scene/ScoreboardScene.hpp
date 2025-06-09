#ifndef SCOREBOARD_SCENE_HPP
#define SCOREBOARD_SCENE_HPP

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/color.h>

struct PlayerData{
    std::string name;
    int score=0;
    std::string timeinfo;
    Engine::Label* rankLabel=nullptr,* nameLabel=nullptr,* scoreLabel=nullptr,* timeLabel=nullptr;
};
struct rgbColor {
    int r, g, b;
};

class ScoreboardScene : public Engine::IScene {
public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void ToPage(int page);
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
    void PrevOnClick();
    void NextOnClick();
    void Update(float deltaTime) override;

private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int w=Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h=Engine::GameEngine::GetInstance().GetScreenSize().y;
    Engine::Label *UIscoreboardTitle, *UIrank, *UIname, *UIscore, *UItime, *UIpage;

    const int cUp = 225;
    const int cLow = 175;
    const int defaultColorIdx = 8;
    std::vector<rgbColor> co;
    float ticks;
    void updateColor();

    int page;
    std::vector<PlayerData> playerDataList;
    void removePlayerLabels();

    float selectionBoxX = 0, selectionBoxY = 0;
    void DeletePlayerData(int rank);
    void LoadPlayerData();
    void OnMouseDown(int, int, int) override;
    void Draw() const override;
    int deleteRank = -1; // Rank to delete, -1 means no deletion
};

#endif // SCOREBOARD_SCENE_HPP