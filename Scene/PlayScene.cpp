#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/RedcarEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "Turret/SniperTurret.hpp"
#include "Turret/ShovelTool.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Animation/RainEffect.hpp"
#include <algorithm>
// #include "Tool/ShovelTool.hpp"
// #include "Tool/ToolButton.hpp"

// TODO HACKATHON-4 (1/3): Trace how the game handles keyboard input.
// TODO HACKATHON-4 (2/3): Find the cheat code sequence in this file.
// TODO HACKATHON-4 (3/3): When the cheat code is entered, a plane should be spawned and added to the scene.
// TODO HACKATHON-5 (1/4): There's a bug in this file, which crashes the game when you win. Try to find it.
// TODO HACKATHON-5 (2/4): The "LIFE" label are not updated when you lose a life. Try to fix it.

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { 
    Engine::Point(1, 0), Engine::Point(0, 1), Engine::Point(-1, 0), Engine::Point(0, -1),
    Engine::Point(1, 1), Engine::Point(1, -1), Engine::Point(-1, 1), Engine::Point(-1, -1) };
int PlayScene::MapWidth , PlayScene::MapHeight;
int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
//const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
Engine::Point PlayScene::EndGridPoint;
const std::vector<int> PlayScene::cheatcode = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_LSHIFT, ALLEGRO_KEY_ENTER
};
const std::vector<int> PlayScene::wincode = { ALLEGRO_KEY_W, ALLEGRO_KEY_I, ALLEGRO_KEY_N };
const std::vector<int> PlayScene::losecode = { ALLEGRO_KEY_L, ALLEGRO_KEY_O, ALLEGRO_KEY_S, ALLEGRO_KEY_E };
const std::vector<int> PlayScene::raincode = { ALLEGRO_KEY_R, ALLEGRO_KEY_A, ALLEGRO_KEY_I,ALLEGRO_KEY_N };
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 100;
    money = 150;
    SpeedMult = 1;
    golds = 0;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    Engine::Point screenSize = Engine::GameEngine::GetInstance().GetScreenSize();
    Engine::Image* background = new Engine::Image("play/sky.png", // 使用現有地圖樣式作為背景
                                                  0, 0,
                                                  screenSize.x, screenSize.y);
    background->followCamera = false; // 背景不跟隨攝影機，固定在螢幕上
    TileMapGroup->AddNewObject(background);
    ReadMap();
    // this->BlockSize = 64;
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    imgTarget->followCamera = true;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    effectcastle = new Engine::Sprite("play/castle.png", MapWidth * BlockSize / 2, (MapHeight - 2) * BlockSize / 2, BlockSize * 6, BlockSize * 6);
    effectcastle->followCamera = true;
    TowerGroup->AddNewObject(effectcastle);

    arrow = new Engine::Image("play/bullet-4.png", (uiBoundaryX + screenSize.x) /2, screenSize.y / 2, 80, 80, 0.5, 0.5);
    arrow->isArrow = true;
    UIGroup->AddNewObject(arrow);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");

    Engine::Point playerInitialPosition = Engine::Point(MapWidth * BlockSize / 2.0f, MapHeight * BlockSize / 2.0f);
    player = new Player("images/play/Player2.png", playerInitialPosition, 400.0f, 0.1f); // camera setted in Player::Update
    Engine::LOG(Engine::LogType::DEBUGGING) << "Camera Position: " << camera.x << ", " << camera.y;
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    Engine::Point distance = player->position - EndGridPoint*BlockSize;
    if(distance.Magnitude() < 400) showHealthBar = true;
    else showHealthBar = false;
    arrow->Visible=!showHealthBar;
    if(arrow){
        float dx =EndGridPoint.x*BlockSize - player->position.x;
        float dy =EndGridPoint.y*BlockSize - player->position.y;
        float angle = atan2(dy, dx) + ALLEGRO_PI / 2;
        arrow->angle = angle; // 更新箭頭的角度
    }

    fpsTicks += deltaTime;
    if (fpsTicks >= 0.5f) {
        fpsTicks -= 0.5f;
        UISfps->Text = "FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime));
    }
    // If we use deltaTime directly, then we might have Bullet-through-paper problem. Reference: Bullet-Through-Paper
    if (SpeedMult == 0) deathCountDown = -1;
    else if (deathCountDown != -1) SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto &it : EnemyGroup->GetObjects()) {
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes) {
        if (it <= DangerTime) {
            danger--;
            if (danger <= 0) {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown) {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0) AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0) {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0) deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++) {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) {
            if (EnemyGroup->GetObjects().empty()) {
                // Free resources.
                /*delete TileMapGroup;
                delete GroundEffectGroup;
                delete DebugIndicatorGroup;
                delete TowerGroup;
                delete EnemyGroup;
                delete BulletGroup;
                delete EffectGroup;
                delete UIGroup;
                delete imgTarget;*/
                // Win.
                Engine::GameEngine::GetInstance().ChangeScene("win");
            }
            continue;
        }
        auto current = enemyWaveData.front();
        if (ticks < current.second)
            continue;
        ticks -= current.second;
        enemyWaveData.pop_front();
        const Engine::Point SpawnGridPoint = GetRandomSpawnPoint();
        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        Enemy *enemy;
        switch (current.first) {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            // TODO HACKATHON-3 (2/3): Add your new enemy here.
            case 2:
                EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 3:
                EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 4:
                EnemyGroup->AddNewObject(enemy = new RedcarEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            default:
                continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
        
    }
    static float rainTimer;
    rainTimer += deltaTime;
    if (isRaining) {
        rainTimer += deltaTime; // 累積時間
        while (rainTimer >= 8.f) { // 確保不會遺漏任何生成
            rainTimer -= 7.f; // 減去間隔
            float x = static_cast<float>(rand()) / RAND_MAX * Engine::GameEngine::GetInstance().GetScreenWidth();
            float y = 0;
            float speedY = 300.0f + static_cast<float>(rand()) / RAND_MAX * 100.0f;
            float lifetime = 4.f + static_cast<float>(rand()) / RAND_MAX * 0.50f;
            EffectGroup->AddNewObject(new RainEffect(x, y, speedY, lifetime));
            Engine::LOG(Engine::LogType::INFO) << "rain generated";
        }
    }

    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }

    player->Update(deltaTime); // Update the player character.
    if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_R] && Engine::GameEngine::GetInstance().isMouseDown) {
        UIBtnClicked(3);
        putThings(1, Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
    }
    // if (Engine::GameEngine::GetInstance().isMouseDown) putThings(1, Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
    // if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S] ||
    //     Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A] || Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D]) {
    //         if        (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_W]) {
    //             camera.y -= cameraSpeed * deltaTime;
    //             // if (camera.y < 0) camera.y = 0;
    //         } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_S]) {
    //             camera.y += cameraSpeed * deltaTime;
    //             // if (camera.y > MapHeight * BlockSize - GetClientSize().y) camera.y = MapHeight * BlockSize - GetClientSize().y;
    //         } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_A]) {
    //             camera.x -= cameraSpeed * deltaTime;
    //             // if (camera.x < 0) camera.x = 0;
    //         } else if (Engine::GameEngine::GetInstance().keyStates[ALLEGRO_KEY_D]) {
    //             camera.x += cameraSpeed * deltaTime;
    //             // if (camera.x > MapWidth * BlockSize - GetClientSize().x) camera.x = MapWidth * BlockSize - GetClientSize().x;
    //         }
    //         std::cout << "Camera Position: (" << camera.x << ", " << camera.y << ")" << std::endl;
    // }
}
Engine::Point PlayScene::GetRandomSpawnPoint() const {
    std::vector<Engine::Point> candidates;
    // int cx = MapWidth / 2;
    // int cy = MapHeight / 2;
    // for (int y = -1; y <= MapHeight; ++y) {
    //     for (int x = -1; x <= MapWidth; ++x) {
    //         int dist = std::abs(x - cx) + std::abs(y - cy);
    //         if (dist < 10) continue;
    //         candidates.emplace_back(x, y);
    //     }
    // }
    for (int x = 0 ; x < MapWidth ; ++x) {
        candidates.emplace_back(x, -1);
        candidates.emplace_back(x, MapHeight);
    }
    for (int y = 0 ; y < MapHeight ; ++y) {
        candidates.emplace_back(-1, y);
        candidates.emplace_back(MapWidth, y);
    }
    if (candidates.empty()) return Engine::Point(0, 0);
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> distIdx(0, candidates.size() - 1);
    return candidates[distIdx(rng)];
}
void PlayScene::Draw() const {
    IScene::Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1 || 1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Point drawPos((j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    // if (drawPos.x < -BlockSize || drawPos.x > Engine::GameEngine::GetInstance().GetScreenWidth() + BlockSize ||
                    //     drawPos.y < -BlockSize || drawPos.y > Engine::GameEngine::GetInstance().GetScreenHeight() + BlockSize) {
                    //     continue; // Skip drawing if out of bounds.
                    // }
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, drawPos.x, drawPos.y);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.followCamera = true;
                    // DebugIndicatorGroup->AddNewObject(&label);
                    label.Draw();
                }
            }
        }
    }
    player->Draw(); // Draw the player character.
    if(showHealthBar) DrawHealthBar();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);

    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if (canReachInteract(x, y, mx, my)) {
        if (mapState[y][x] == TILE_GOLD) {

        }
        mapState[y][x] = PlayScene::TILE_DIRT;
        TileMapGroup->RemoveObject(TileMapImages[y][x]->GetObjectIterator());
        TileMapImages[y][x] = new Engine::Image("play/grass2.png", x * PlayScene::BlockSize, y * PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize);
        TileMapImages[y][x]->followCamera = true;
        TileMapGroup->AddNewObject(TileMapImages[y][x]);
    }
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight || mx >= uiBoundaryX) {
        imgTarget->Visible = false;
    } else {
        imgTarget->Visible = true;
        imgTarget->Position.x = x * BlockSize;
        imgTarget->Position.y = y * BlockSize;
    }
    for (int i = -3; i <= 3; i++) {
        for (int j=-3;j<=3;j++) {
            int nx=x+i,ny=y+j;
            if(nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight || mx >= uiBoundaryX) continue;
            TileMapImages[ny][nx]->color = al_map_rgb(255, 255, 255);
        }
    }
    if (canReachInteract(x, y, mx, my)) {
        // Engine::LOG(Engine::LogType::INFO) << x << ' ' << y << ' ' << "canInteract";
        // std::cout << x * BlockSize - camera.x << std::endl;
        TileMapImages[y][x]->color = al_map_rgb(200, 200, 255);
    }
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    putThings(button, mx, my);
    // if (!imgTarget->Visible)
    //     return;
    // const int x = (mx + camera.x) / BlockSize;
    // const int y = (my + camera.y) / BlockSize;
    // if ((button & 1) && mx < uiBoundaryX) {
    //     if (mapState[y][x] != TILE_OCCUPIED || preview->isTool) {
    //         if (!preview)
    //             return;
    //         // Check if valid.
    //         if (!CheckSpaceValid(x, y)) {
    //             Engine::Sprite *sprite;
    //             sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);
    //             sprite->followCamera = true;
    //             GroundEffectGroup->AddNewObject(sprite);
    //             sprite->Rotation = 0;
    //             return;
    //         }
    //         // Purchase.
    //         EarnMoney(-preview->GetPrice());
    //         //Turret* newpreview = preview;
    //         // Remove Preview.
    //         preview->GetObjectIterator()->first = false;
    //         UIGroup->RemoveObject(preview->GetObjectIterator());
    //         // Construct real turret.
    //         preview->Position.x = x * BlockSize + BlockSize / 2;
    //         preview->Position.y = y * BlockSize + BlockSize / 2;
    //         preview->Enabled = true;
    //         preview->Preview = false;
    //         preview->Tint = al_map_rgba(255, 255, 255, 255);
    //         preview->followCamera = preview->imgBase.followCamera = true;
    //         if (!preview->isTool) {
    //             TowerGroup->AddNewObject(preview);
    //             Towers[y][x] = preview;
    //         } else {
    //             preview->Use();
    //         }
    //         // To keep responding when paused.
    //         preview->Update(0);
    //         if (!preview->isTool) mapState[y][x] = TILE_OCCUPIED;
    //         // Remove Preview.
    //         preview = nullptr;
    //         OnMouseMove(mx, my);
    //     }
    // }
}
void PlayScene::putThings(int button, int mx, int my) {
    if (!imgTarget->Visible)
        return;
    const int x = (mx + camera.x) / BlockSize;
    const int y = (my + camera.y) / BlockSize;
    if ((button & 1) && mx < uiBoundaryX) {
        if (mapState[y][x] == TILE_FLOOR || preview->isTool || 1) { // deal in CheckSpaceValid
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y)) {
                Engine::Sprite *sprite;
                sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);
                sprite->followCamera = true;
                GroundEffectGroup->AddNewObject(sprite);
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            //Turret* newpreview = preview;
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);

            preview->followCamera = preview->imgBase.followCamera = true;
            if (!preview->isTool) {
                TowerGroup->AddNewObject(preview);
                Towers[y][x] = preview;
            } else {
                preview->Use();
            }
            // To keep responding when paused.
            preview->Update(0);
            if (!preview->isTool) mapState[y][x] = TILE_OCCUPIED;

            // Remove Preview.
            preview = nullptr;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    } else {
        keyStrokes.push_back(keyCode);
        if(keyStrokes.size() > 20) keyStrokes.pop_front();

        if ((keyStrokes.size() >= cheatcode.size() && 
             (std::equal(std::prev(keyStrokes.end(), cheatcode.size()), keyStrokes.end(), cheatcode.begin()))) ||
            keyCode == ALLEGRO_KEY_ENTER) {
                std::cout << "Cheat code entered!" << std::endl;
                UIGroup->AddNewObject(new Plane());
                EarnMoney(10000);
        }
        if (keyStrokes.size() >= wincode.size() && 
            std::equal(std::prev(keyStrokes.end(), wincode.size()), keyStrokes.end(), wincode.begin())) {
                std::cout << "Win code entered!" << std::endl;
                Engine::GameEngine::GetInstance().ChangeScene("win");
        }
        if (keyStrokes.size() >= losecode.size() && 
            std::equal(std::prev(keyStrokes.end(), losecode.size()), keyStrokes.end(), losecode.begin())) {
                std::cout << "Lose code entered!" << std::endl;
                Engine::GameEngine::GetInstance().ChangeScene("lose");
        }
        if (keyStrokes.size() >= raincode.size() && 
            std::equal(std::prev(keyStrokes.end(),raincode.size()), keyStrokes.end(), raincode.begin())) {
            std::cout << "rain code entered!" << std::endl;
            
            if (!isRaining) {
                isRaining = true; // 防止重複啟動。
                std::cout << "Rain effect activated!" << std::endl;
            } else isRaining = false;
                
        }
    }

    if( keyCode == ALLEGRO_KEY_ESCAPE) {
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    } else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = (keyCode - ALLEGRO_KEY_0);
    } else if (keyCode == ALLEGRO_KEY_PAD_0) {
        SpeedMult = 25;
    } else if (keyCode == ALLEGRO_KEY_PAD_PLUS) {
        player->speed *= 1.5f;
    } else if (keyCode == ALLEGRO_KEY_PAD_MINUS) {
        player->speed /= 1.5f;
    }
    // else if (keyCode == ALLEGRO_KEY_Q) {
    //     // Hotkey for MachineGunTurret.
    //     UIBtnClicked(0);
    // } else if (keyCode == ALLEGRO_KEY_W) {
    //     // Hotkey for LaserTurret.
    //     UIBtnClicked(1);
    // } else if (keyCode == ALLEGRO_KEY_E) {
    //     // Hotkey for SniperTurret.
    //     UIBtnClicked(2);
    // } else if (keyCode == ALLEGRO_KEY_R) {
    //     // Hotkey for ShovelTool.
    //     UIBtnClicked(3);
    // } 
    player->OnKeyDown(keyCode);
}
void PlayScene::Hit() {
    lives--;
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
    UILives->Text = std::string("Life ") + std::to_string(lives);
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoneyLabel->Text = std::to_string(this->money);
}
void PlayScene::ReadMap() {
    //test in stage3 combine 1 2
    if(MapId == 3 || MapId == 4) ReadSpecialMap(MapId);
    else {
        std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
        // Check if the file exists.
        std::ifstream fileCheck(filename);
        if (!fileCheck) Engine::LOG(Engine::LogType::ERROR) << "Map file not found: " << filename;

        // Read map file.
        std::ifstream fin(filename);
        std::string line;
        std::vector<std::string> lines;
        while(std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            lines.push_back(line);
        }    
        fin.close();

        MapHeight=lines.size();
        MapWidth=lines.empty() ? 0 : lines[0].size();
        EndGridPoint= Engine::Point(MapWidth /2, MapHeight /2);

        // Validate map data.
        for (const auto& l : lines) {
            if (l.size() != MapWidth) Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: inconsistent row length.";
        }

        // Store map in 2d array.
        mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
        TileMapImages = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
        Towers = std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth));
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                char c = lines[i][j];
                readMapTiles(i, j, c);
                // if (c == '0') {
                //     mapState[i][j]=TILE_DIRT;
                //     TileMapImages[i][j] = (new Engine::Image("play/grass2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // } else if(c == '1') {
                //     mapState[i][j]=TILE_FLOOR;
                //     TileMapImages[i][j] = (new Engine::Image("play/rock.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // }else if(c == '2') {
                //     mapState[i][j]=TILE_OBSTACLE;
                //     TileMapImages[i][j] = (new Engine::Image("play/rock_grass.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // }else if(c == '3') {
                //     mapState[i][j]=TILE_OBSTACLE;
                //     TileMapImages[i][j] = (new Engine::Image("play/flowers3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // }
                TileMapImages[i][j]->followCamera = true;
                TileMapGroup->AddNewObject(TileMapImages[i][j]);
            }
        }
    }
    //print mapstate
    Engine::LOG(Engine::LogType::INFO) << "Map State:";
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            if (mapState[i][j] == TILE_DIRT) std::cout << "0 ";
            else std::cout << mapState[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
void PlayScene::ReadSpecialMap(int mapId) {
    if(mapId == 3) {
        std::string filename1 = std::string("Resource/map1.txt");
        std::string filename2 = std::string("Resource/map2.txt");
        std::ifstream fin1(filename1);
        std::ifstream fin2(filename2);
        if (!fin1.is_open()) Engine::LOG(Engine::LogType::ERROR) << "Map file not found: " << filename1;
        if (!fin2.is_open()) Engine::LOG(Engine::LogType::ERROR) << "Map file not found: " << filename2;

        // Read map file.
        std::string line1, line2;
        std::vector<std::string> lines1, lines2;
        while(std::getline(fin1, line1)) {
            if (line1.empty() || line1[0] == '#') continue;
            lines1.push_back(line1);
        }
        while(std::getline(fin2, line2)) {
            if (line2.empty() || line2[0] == '#') continue;
            lines2.push_back(line2);
        }
        fin1.close();
        fin2.close();

        MapWidth=lines1[0].size() + lines2[0].size();
        MapHeight=std::max(lines1.empty() ? 0 : lines1.size(), lines2.empty() ? 0 : lines2.size());
        EndGridPoint = Engine::Point(MapWidth / 2, MapHeight / 2);
        Engine::LOG(Engine::LogType::INFO) << "MapId: " << MapId << ", MapWidth: " << MapWidth << ", MapHeight: " << MapHeight;

        mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
        TileMapImages = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
        Towers = std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth));
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                char c = '\0';
                if (j < lines1[i].size()) c = lines1[i][j];
                else if (i < lines2.size() && j >= lines1[i].size()) c = lines2[i][j - lines1[i].size()];
                else Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: inconsistent row length.";
                readMapTiles(i, j, c);
                // if (c == '\0') Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: empty tile at " << Engine::Point(i, j);
                // else if (c == '0') {
                //     mapState[i][j]=TILE_DIRT;
                //     TileMapImages[i][j] = (new Engine::Image("play/grass2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // } else if(c == '1') {
                //     mapState[i][j]=TILE_FLOOR;
                //     TileMapImages[i][j] = (new Engine::Image("play/rock.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // } else if(c == '2') {
                //     mapState[i][j]=TILE_OBSTACLE;
                //     TileMapImages[i][j] = (new Engine::Image("play/rock_grass.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // } else if(c == '3') {
                //     mapState[i][j]=TILE_OBSTACLE;
                //     TileMapImages[i][j] = (new Engine::Image("play/flowers3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // }
                TileMapImages[i][j]->followCamera = true;
                TileMapGroup->AddNewObject(TileMapImages[i][j]);

                // if (/*i < lines1.size() &&*/ j < lines1[i].size()) {
                //     // Use line1 data.
                //     char c = lines1[i][j];
                //     if (c == '0') {
                //         mapState[i][j]=TILE_DIRT;
                //         TileMapImages[i][j] = (new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     } else if(c == '1') {
                //         mapState[i][j]=TILE_FLOOR;
                //         TileMapImages[i][j] = (new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     }
                //     TileMapImages[i][j]->followCamera = true;
                //     TileMapGroup->AddNewObject(TileMapImages[i][j]);
                // } else if (i < lines2.size() && j >= lines1[i].size()) {
                //     // Use line2 data.
                //     char c = lines2[i][j-lines1[i].size()];
                //     if (c == '0') {
                //         mapState[i][j]=TILE_DIRT;
                //         TileMapImages[i][j] = (new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     } else if(c == '1') {
                //         mapState[i][j]=TILE_FLOOR;
                //         TileMapImages[i][j] = (new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     }
                //     TileMapImages[i][j]->followCamera = true;
                //     TileMapGroup->AddNewObject(TileMapImages[i][j]);
                // }
                // else /*if (i >= lines1.size() && j < lines2[i].size()*/) {
                //     // Use line2 data.
                //     char c = lines2[i][j-lines1[i].size()];
                //     if (c == '0') {
                //         mapState[i][j]=TILE_DIRT;
                //         TileMapImages[i][j] = (new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     } else if(c == '1') {
                //         mapState[i][j]=TILE_FLOOR;
                //         TileMapImages[i][j] = (new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                //     }
                //     TileMapImages[i][j]->followCamera = true;
                //     TileMapGroup->AddNewObject(TileMapImages[i][j]);
                // }
            }
        }
    }
    else if (mapId == 4) { // map height should equal
        const int mapNumber = 3;

        std::vector<std::vector<std::string>> lines(mapNumber + 1);
        // std::vector<std::string> lines1, lines2;
        for (int i = 1; i <= mapNumber; i++) {
            std::string filename= std::string("Resource/map") + std::to_string(i) + ".txt";
            std::ifstream fin;
            fin.open(filename);
            if (!fin.is_open()) Engine::LOG(Engine::LogType::ERROR) << "Map file not found: " << filename;
            
            std::string line;
            while (std::getline(fin, line)) {
                if (line.empty() || line[0] == '#') continue;
                lines[i].push_back(line);
                // if (i==1) lines1.push_back(line);
                // else if (i==2) lines2.push_back(line);
            }
            Engine::LOG(Engine::LogType::INFO) << "MapId: " << MapId << ", Read map file: " << filename;
            fin.close();
        }
        // for (const auto& f : filename) {
        //     fin.emplace_back(f);
        //     if (!fin.back().is_open()) Engine::LOG(Engine::LogType::ERROR) << "Map file not found: " << f;
        // }
        // Read map file.
        //std::vector<std::string> line;
        // std::string line1, line2;
        // std::vector<std::string> lines1, lines2;
        // while(std::getline(fin[0], line1)) {
        //     if (line1.empty() || line1[0] == '#') continue;
        //     lines1.push_back(line1);
        // }
        // while(std::getline(fin[1], line2)) {
        //     if (line2.empty() || line2[0] == '#') continue;
        //     lines2.push_back(line2);
        // }
        // fin[0].close();
        // fin[1].close();

        //const int mapCombineW = 3, mapCombineH = 4;
        std::vector<std::vector<int>> mapCombineTable(mapCombineH, std::vector<int>(mapCombineW));
        std::vector<int> candidates(mapNumber);
        for(int i = 0; i < candidates.size(); i++) candidates[i] = i + 1;

        MapWidth = 0;
        int tmpWidth;
        for (int i = 0; i < mapCombineH; i++) {
            tmpWidth = 0;
            for (int j = 0; j < mapCombineW; j++) {
                std::random_device rd;
                std::mt19937 rng(rd());
                std::uniform_int_distribution<> distIdx(0, candidates.size() - 1);
                mapCombineTable[i][j] = candidates[distIdx(rng)];
                // tmpWidth += (mapCombineTable[i][j] == 1 ? lines1[0].size() : lines2[0].size());
                tmpWidth += lines[mapCombineTable[i][j]][0].size();
            }
            MapWidth = std::max(MapWidth, tmpWidth);
        }
        MapHeight = lines[mapCombineTable[0][0]].size() * mapCombineH;
        
        EndGridPoint = Engine::Point(MapWidth / 2, MapHeight / 2);
        Engine::LOG(Engine::LogType::INFO) << "MapId: " << MapId << ", MapWidth: " << MapWidth << ", MapHeight: " << MapHeight;

        mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
        TileMapImages = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
        Towers = std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth));

        // 4 for loop to read map data.
        Engine::Point accmulate(0, 0);
        for (int i = 0; i < mapCombineH; i++) {
            accmulate.x = 0;
            for (int j = 0; j < mapCombineW; j++) {
                if (mapCombineTable[i][j] < 1 || mapCombineTable[i][j] > mapNumber) {
                    Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: invalid map combine table.";
                    continue;
                }
                std::vector<std::string> useLines = lines[mapCombineTable[i][j]];
                // if (mapCombineTable[i][j] == 1) useLines = lines1;
                // else if (mapCombineTable[i][j] == 2) useLines = lines2;
                // else {
                //     Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: invalid map combine table.";
                //     continue;
                // }
                for (int y = 0; y < useLines.size(); y++) {
                    for (int x = 0; x < useLines[y].size(); x++) {
                        char c = useLines[y][x];
                        readMapTiles(y + accmulate.y, x + accmulate.x, c);
                        // if (c == '\0') Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: empty tile at " << Engine::Point(j, i);
                        // else if (c == '0') {
                        //     mapState[y + accmulate.y][x + accmulate.x] = TILE_DIRT;
                        //     TileMapImages[y + accmulate.y][x + accmulate.x] = (new Engine::Image("play/grass2.png", (x + accmulate.x) * BlockSize, (y + accmulate.y) * BlockSize, BlockSize, BlockSize));
                        // } else if (c == '1') {
                        //     mapState[y + accmulate.y][x + accmulate.x] = TILE_FLOOR;
                        //     TileMapImages[y + accmulate.y][x + accmulate.x] = (new Engine::Image("play/rock.png", (x + accmulate.x) * BlockSize, (y + accmulate.y) * BlockSize, BlockSize, BlockSize));
                        // } else if (c == '2') {
                        //     mapState[y + accmulate.y][x + accmulate.x] = TILE_OBSTACLE;
                        //     TileMapImages[y + accmulate.y][x + accmulate.x] = (new Engine::Image("play/rock_grass.png", (x + accmulate.x) * BlockSize, (y + accmulate.y) * BlockSize, BlockSize, BlockSize));
                        // } else if (c == '3') {
                        //     mapState[y + accmulate.y][x + accmulate.x] = TILE_OBSTACLE;
                        //     TileMapImages[y + accmulate.y][x + accmulate.x] = (new Engine::Image("play/flowers3.png", (x + accmulate.x) * BlockSize, (y + accmulate.y) * BlockSize, BlockSize, BlockSize));
                        // }
                        TileMapImages[y + accmulate.y][x + accmulate.x]->followCamera = true;
                        TileMapGroup->AddNewObject(TileMapImages[y + accmulate.y][x + accmulate.x]);
                    }
                }
                accmulate.x += useLines[j].size();
            }
            accmulate.y += lines[mapCombineTable[i][0]].size();
        }
    }
}
void PlayScene::readMapTiles(int y, int x, char c) {
    if (c == '\0') Engine::LOG(Engine::LogType::ERROR) << "Map data is corrupted: empty tile at " << Engine::Point(x, y);
    else if (c == '0') {
        mapState[y][x] = TILE_DIRT;
        TileMapImages[y][x] = (new Engine::Image("play/grass2.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    } else if (c == '1') {
        mapState[y][x] = TILE_FLOOR;
        TileMapImages[y][x] = (new Engine::Image("play/rock.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    } else if (c == '2') {
        mapState[y][x] = TILE_OBSTACLE;
        TileMapImages[y][x] = (new Engine::Image("play/rock_grass.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    } else if (c == '3') {
        mapState[y][x] = TILE_OBSTACLE;
        TileMapImages[y][x] = (new Engine::Image("play/flowers3.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    } else if (c == '4') {
        mapState[y][x] = TILE_WATER;
        TileMapImages[y][x] = (new Engine::Image("play/water.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    } else if (c == '5') {
        mapState[y][x] = TILE_GOLD;
        TileMapImages[y][x] = (new Engine::Image("play/gold.png", x * BlockSize, y * BlockSize, BlockSize, BlockSize));
    }
}
void PlayScene::ReadEnemyWave() {
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI() {
    // Background
    Engine::Image* boundary=new Engine::Image("play/floor.png", uiBoundaryX, 0, 320, 832);
    boundary->color = al_map_rgba(255, 255, 255, 200);
    UIGroup->AddNewObject(boundary);
    // Text
    if(MapId==4) UIGroup->AddNewObject(new Engine::Label(std::string("Customized"), "pirulen.ttf", 32, uiBoundaryX + 14, 0));
    else UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, uiBoundaryX + 14, 0));
    UIGroup->AddNewObject(UIMoneyImage = new Engine::Image("play/coin_icon.png", uiBoundaryX + 12, 40, 48, 48));
    UIGroup->AddNewObject(UIMoneyLabel = new Engine::Label(std::to_string(money), "pirulen.ttf", 24, uiBoundaryX + 60 , 48));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, uiBoundaryX + 14, 88));
    UIGroup->AddNewObject(new Engine::Image("play/gold_ingot.png", uiBoundaryX + 160, 40, 48, 48));
    UIGroup->AddNewObject(UIGoldLabel = new Engine::Label(std::to_string(golds), "pirulen.ttf", 24, uiBoundaryX + 220 , 48));

    TurretButton *btn;
    const int bs=76;// Button size
    // Button 1
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", uiBoundaryX + 14, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", uiBoundaryX + 14, 136 - 8, 0, 0, 0, 0), uiBoundaryX + 14, 136, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", uiBoundaryX + 14 + bs, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", uiBoundaryX + 14 + bs, 136 - 8, 0, 0, 0, 0), uiBoundaryX + 14 + bs, 136, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);

    //Button 3
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/tower-base.png", uiBoundaryX + 14 + bs * 2, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-7.png", uiBoundaryX + 14 + bs * 2, 136 - 8, 0, 0, 0, 0), uiBoundaryX + 14 + bs * 2, 136, SniperTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    //Button 4
    btn = new TurretButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/shovel-base.png", uiBoundaryX + 14 + bs * 3, 136, 0, 0, 0, 0),
                           Engine::Sprite("play/shovel.png", uiBoundaryX + 14 + bs * 3, 136 - 8, 0, 0, 0, 0), uiBoundaryX + 14 + bs * 3, 136, ShovelTool::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
    btn->Base.Visible = false;
    UIGroup->AddNewControlObject(btn);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);

    UISfps = new Engine::Label("FPS: 0", "pirulen.ttf", 24, uiBoundaryX + 14, h - 48);
    UIGroup->AddNewObject(UISfps);
}
void PlayScene::UIBtnClicked(int id) {
    Turret* new_preview = nullptr;

    if (id == 0 && money >= MachineGunTurret::Price)
        new_preview = new MachineGunTurret(0, 0);
    else if (id == 1 && money >= LaserTurret::Price)
        new_preview = new LaserTurret(0, 0);
    else if (id == 2 && money >= SniperTurret::Price)
        new_preview = new SniperTurret(0, 0);
    else if (id == 3 && money >= ShovelTool::Price){
        new_preview = new ShovelTool(0, 0);
        new_preview->imgBase.Visible = false;
    }
    if (!new_preview) return;
    if(preview) UIGroup->RemoveObject(preview->GetObjectIterator());
    preview = new_preview;

    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}
bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) return false;
    if (!preview->isTool) {
        return mapState[y][x] == TILE_FLOOR;
    } 
    else if(preview->isTool) {
        return (mapState[y][x] == TILE_FLOOR || mapState[y][x] == TILE_OCCUPIED);
    }
    else return false; // not occur
    // auto map00 = mapState[y][x];
    // mapState[y][x] = TILE_OCCUPIED;
    // std::vector<std::vector<int>> map = CalculateBFSDistance();
    // mapState[y][x] = map00;
    // if (map[0][0] == -1) return false;
    // for (auto &it : EnemyGroup->GetObjects()) {
    //     Engine::Point pnt;
    //     pnt.x = floor(it->Position.x / BlockSize);
    //     pnt.y = floor(it->Position.y / BlockSize);
    //     if (pnt.x < 0) pnt.x = 0;
    //     if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
    //     if (pnt.y < 0) pnt.y = 0;
    //     if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
    //     if (map[pnt.y][pnt.x] == -1)
    //         return false;
    // }
    // // ^All enemy have path to exit.
    // //// mapState[y][x] = TILE_OCCUPIED;
    // mapDistance = map;
    // for (auto &it : EnemyGroup->GetObjects())
    // //     dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    // return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[EndGridPoint.y][EndGridPoint.x] != TILE_DIRT) {
        std::cout << EndGridPoint.x << " " << EndGridPoint.y << ": " <<
            mapState[EndGridPoint.y][EndGridPoint.x] << " not dirt" << std::endl;
        return map;
    }
    que.push(EndGridPoint);
    map[EndGridPoint.y][EndGridPoint.x] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        // TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
        const int dx[4]={1,0,-1,0};
        const int dy[4]={0,1,0,-1};
        for (int i=0;i<4;i++) {
            int nx=p.x+dx[i];
            int ny=p.y+dy[i];
            if (nx<0||nx>=MapWidth||ny<0||ny>=MapHeight) continue;
            if (map[ny][nx]==-1&&mapState[ny][nx]==TILE_DIRT) {
                map[ny][nx]=map[p.y][p.x]+1;
                que.push(Engine::Point(nx,ny));
            }
        }
    }
    return map;
}
bool PlayScene::canWalk(Engine::Point pos) const {
    pos.x = static_cast<int>(pos.x);
    pos.y = static_cast<int>(pos.y);
    if (pos.x < 0 || pos.x >= MapWidth || pos.y < 0 || pos.y >= MapHeight) return false;
    return mapState[pos.y][pos.x] == TILE_DIRT || mapState[pos.y][pos.x] == TILE_FLOOR || 
        mapState[pos.y][pos.x] == TILE_OCCUPIED || mapState[pos.y][pos.x] == TILE_WATER ||
        mapState[pos.y][pos.x] == TILE_GOLD;
}
bool PlayScene::canReachInteract(int x, int y, float mx, float my) const {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) return false;
    return canInteract(x, y) && (Engine::Point(mx, my) + camera - player->position).Magnitude() <= 5 * BlockSize;
}
bool PlayScene::canInteract(int x, int y) const {
    return mapState[y][x] == TILE_GOLD;
}
void PlayScene::DrawHealthBar() const {
    const int barWidth = 170;
    const int barHeight = 24;
    const float healthRatio = static_cast<float>(lives) / 100.0f;

    int barX = EndGridPoint.x * BlockSize - barWidth / 2 - camera.x ;
    int barY = EndGridPoint.y * BlockSize - barHeight / 2 - 35 - camera.y; 

    //血量
    al_draw_filled_rectangle(
        barX,
        barY,
        barX + barWidth * healthRatio,
        barY + barHeight,
        al_map_rgb(255, 0, 0)
    );

    //邊框
    al_draw_rectangle(
        barX,
        barY,
        barX + barWidth,
        barY + barHeight,
        al_map_rgb(0, 0, 0),
        4.0f
    );
}
