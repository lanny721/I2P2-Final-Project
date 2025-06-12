#ifndef MAPSELECTSCENE_HPP
#define MAPSELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
   // namespace Engine

class MapSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int customizedW=3, customizedH=4;
    int cusWEntered = 0, cusHEntered = 0;
protected:

public:
    explicit MapSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int id);
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
    void DrawPreviewMap() ;
    void DrawCustomize() ;
    int MapId;
};
#endif   // MAPSELECTSCENE_HPP
