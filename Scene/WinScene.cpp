#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"
#include "allegro5/allegro_primitives.h"//changed
#include "Scene/ScoreboardScene.hpp"//changed

void WinScene::Initialize() {
    ticks = 0;
    //changed
    waitingForName = true;
    playerName = "";
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH+30, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 15, 255, 255, 255, 255, 0.5, 0.5));
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

//changed
void WinScene::Draw() const {
    IScene::Draw();
    if (waitingForName) {
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
        int boxY = h/2 - 250; // Move box up by 70 pixels
        // Draw rectangle
        al_draw_filled_rectangle(w/2 - 150, boxY - 50, w/2 + 150, boxY + 50, al_map_rgb(50, 50, 50));
        al_draw_rectangle(w/2 - 150, boxY - 50, w/2 + 150, boxY + 50, al_map_rgb(200, 200, 200), 2);
        // Draw static prompt
        Engine::Label namePrompt("Enter your name:", "pirulen.ttf", 28, w/2 -550, boxY, 255, 255, 255, 255, 0, 0.5);
        namePrompt.Draw();
        // Draw dynamic name (fixed position)
        Engine::Label nameLabel(playerName + "_", "pirulen.ttf", 28, w/2 -130, boxY, 255, 255, 255, 255, 0, 0.5);
        nameLabel.Draw();
    }
}

void WinScene::OnKeyDown(int keyCode) {
    if (!waitingForName) {
        IScene::OnKeyDown(keyCode);
        return;
    }
    if (keyCode == ALLEGRO_KEY_BACKSPACE) {
        if (!playerName.empty())
            playerName.pop_back();
    } else if (keyCode == ALLEGRO_KEY_ENTER || keyCode == ALLEGRO_KEY_PAD_ENTER) {
        if (!playerName.empty()) {
            // Pass name and score to ScoreboardScene
            auto scoreboard = dynamic_cast<ScoreboardScene*>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
            auto playScene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
            if (scoreboard && playScene) {
                scoreboard->AddScore(/*lives*/playScene->Getlives(), /*money*/playScene->GetMoney(), /*cheats*/playScene->GetchatesUsed(), playerName); // Pass actual values
            }
            waitingForName = false;
            Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
        }
    } else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
        if (playerName.length() < 10) {
            char c = 'A' + (keyCode - ALLEGRO_KEY_A);
            playerName += c;
        }
    } else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        if (playerName.length() < 10) {
            char c = '0' + (keyCode - ALLEGRO_KEY_0);
            playerName += c;
        }
    } else if (keyCode == ALLEGRO_KEY_SPACE) {
        if (playerName.length() < 10 && !playerName.empty() && playerName.back() != ' ') {
            playerName += ' ';
        }
    }
}