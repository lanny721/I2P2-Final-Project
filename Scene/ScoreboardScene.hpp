#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Sprite.hpp"

class ScoreboardScene final : public Engine::IScene {
private:
    Group* UIGroup = nullptr;

    int curPage=1;
    int totalPage;
    std::vector<Engine::Label*> scoreLabels;
    void UpdateScoreboardDisplay();

    int selectedIndex = -1;  // 記錄被選中的紀錄索引
    ALLEGRO_BITMAP* selectionBox = nullptr;
    float selectionBoxX, selectionBoxY;
    std::vector<std::tuple<std::string, int, std::string>> scores;  // 移到類成員
public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void NextOnClick(int stage);
    void PrevOnClick(int stage);
    void BackOnClick(int stage);

    void getScoreboard(std::vector<std::tuple<std::string, int, std::string>>& scores);

    //for delete records
    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void Draw() const override;
};

#endif   // SCOREBOARDSCENE_HPP