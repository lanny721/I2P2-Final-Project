#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "Player/Player1.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

class Turret;
class Tool;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class PlayScene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    Player* player;
    int uiBoundaryX = 1280; // UI boundary X coordinate, used to determine if the mouse is in the UI area.
    float fpsTicks = 0.f; // Ticks for FPS calculation.
    float raintick;
    void ReadSpecialMap(int mapId);
    void readMapTiles(int y, int x, char c);
    void putThings(int button, int mx, int my);
    bool showHealthBar = false;
    Engine::Image* skyBackground; // 背景圖片。

protected:
    int lives;
    int money;
    int SpeedMult;
    int golds;

public:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_STONE,
        TILE_FLOWER,
        TILE_WATER,
        TILE_GOLD,
        TILE_DOOR,
        TILE_OBSTACLE, 
        TILE_OCCUPIED
    };

    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int defW = 20, defH = 13; // Default map width and height.
    static int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static Engine::Point EndGridPoint;
    static const std::vector<int> cheatcode;
    static const std::vector<int> wincode;
    static const std::vector<int> losecode;
    static const std::vector<int> raincode;

    int MapId;
    int mapCombineW, mapCombineH, enemyWaveNum;
    float ticks;
    float deathCountDown;
    bool isRaining = false;
    
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *TowerGroup;
    Group *EnemyGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Engine::Label *UIMoneyLabel;
    Engine::Label *UILives;
    Engine::Label *UISfps;
    Engine::Label *UIGoldLabel;
    Engine::Image *UIMoneyImage;
    Engine::Image *imgTarget;
    Engine::Image *arrow;
    Engine::Image *healthBar= nullptr;
    Engine::Sprite *dangerIndicator;
    Engine::Sprite *effectcastle;
    Turret *preview;
    std::vector<std::vector<TileType>> mapState; // [y][x] important!!!!!!!!!!!!!! 
    std::vector<std::vector<int>> mapDistance;
    std::vector<std::vector<Engine::Image*>> TileMapImages; // [y][x]
    std::vector<std::vector<Turret*>> Towers; // [y][x]
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y);
    bool canWalk(Engine::Point pos) const;
    void DrawHealthBar() const;
    bool canInteract(int x, int y) const;
    bool canReachInteract(int x, int y, float mx, float my) const;
    Engine::Point GetRandomSpawnPoint() const;
    std::vector<std::vector<int>> CalculateBFSDistance();
};
#endif   // PLAYSCENE_HPP
