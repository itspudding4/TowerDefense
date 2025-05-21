#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class WinScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;

    //changed 3 lines
    bool waitingForName = false;
    std::string playerName;
    int finalScore = 0;

public:
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;

    void Update(float deltaTime) override;
    void BackOnClick(int stage);

    //changed 2 lines
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
};

#endif   // WINSCENE_HPP
