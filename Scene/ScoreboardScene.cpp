#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "ScoreboardScene.hpp"
#include "PlayScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

void ScoreboardScene::Initialize() {
    // Set default values
    playerName = "";
    waitingForName = false;
    newScore = 0;
    cheatsUsed = 0;
    scoreLabelsId.clear();
    scoreLabels.clear();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Load background
    background = new Engine::Image("stage-select/dirt.png", 0, 0, w, h);
    AddNewObject(background);

    // Create title
    titleLabel = new Engine::Label("SCOREBOARD", "pirulen.ttf", 48, halfW, 80, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(titleLabel);

    // Create back button
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, h - 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    backLabel = new Engine::Label("Back", "pirulen.ttf", 48, halfW, h - 50, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(backLabel);

    // Load scores from file
    LoadScores();

    // Create score labels
    int yPos = 150;

    // Header - Separate columns for Rank, Player, Score
    Engine::Label* rankHeader = new Engine::Label("Rank", "pirulen.ttf", 28, halfW - 350, yPos, 255, 255, 255, 255, 0.0, 0.5);
    Engine::Label* nameHeader = new Engine::Label("Player", "pirulen.ttf", 28, halfW, yPos, 255, 255, 255, 255, 0.5, 0.5);
    Engine::Label* scoreHeader = new Engine::Label("Score", "pirulen.ttf", 28, halfW + 350, yPos, 255, 255, 255, 255, 1.0, 0.5);

    AddNewObject(rankHeader);
    AddNewObject(nameHeader);
    AddNewObject(scoreHeader);
    scoreLabels.push_back(rankHeader);
    scoreLabels.push_back(nameHeader);
    scoreLabels.push_back(scoreHeader);

    yPos += 50;

    // Display top 10 scores
    int displayCount = std::min(10, static_cast<int>(scoreRecords.size()));
    for (int i = 0; i < displayCount; i++) {
        // Rank (left-aligned)
        Engine::Label* rankLabel = new Engine::Label(
            std::to_string(i + 1) + ".",
            "pirulen.ttf", 24, halfW - 300, yPos, 255, 255, 255, 255, 0.0, 0.5
        );

        // Player name (centered)
        Engine::Label* nameLabel = new Engine::Label(
            scoreRecords[i].name,
            "pirulen.ttf", 24, halfW, yPos, 255, 255, 255, 255, 0.5, 0.5
        );

        // Score (right-aligned)
        Engine::Label* scoreLabel = new Engine::Label(
            std::to_string(scoreRecords[i].score),
            "pirulen.ttf", 24, halfW + 300, yPos, 255, 255, 255, 255, 1.0, 0.5
        );

        AddNewObject(rankLabel);
        AddNewObject(nameLabel);
        AddNewObject(scoreLabel);

        scoreLabels.push_back(rankLabel);
        scoreLabels.push_back(nameLabel);
        scoreLabels.push_back(scoreLabel);

        yPos += 40;
    }

    // Start BGM
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardScene::Terminate() {
    // Stop BGM
    if (bgmInstance) {
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }

    // Save scores before closing if needed
    if (waitingForName)
        SaveScores();

    // Clear lists
    scoreLabels.clear();
    scoreLabelsId.clear();

    // Base class terminate
    IScene::Terminate();
}

void ScoreboardScene::Draw() const {
    IScene::Draw();

    if (waitingForName) {
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

        // Draw a semi-transparent overlay
        al_draw_filled_rectangle(0, 0, w, h, al_map_rgba(0, 0, 0, 180));

        // Draw a box for name input
        al_draw_filled_rectangle(w/2 - 300, h/2 - 50, w/2 + 300, h/2 + 50, al_map_rgb(50, 50, 50));
        al_draw_rectangle(w/2 - 300, h/2 - 50, w/2 + 300, h/2 + 50, al_map_rgb(200, 200, 200), 2);

        // Draw the input prompt and current input
        Engine::Label namePrompt("Enter your name: " + playerName + "_", "pirulen.ttf", 28, w/2, h/2, 255, 255, 255, 255, 0.5, 0.5);
        namePrompt.Draw();
    }
}

void ScoreboardScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
}

void ScoreboardScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}

void ScoreboardScene::OnKeyDown(int keyCode) {
    if (!waitingForName) {
        IScene::OnKeyDown(keyCode);
        return;
    }

    // Handle keyboard input for player name
    if (keyCode == ALLEGRO_KEY_BACKSPACE) {
        // Remove the last character
        if (!playerName.empty())
            playerName.pop_back();
    }
    else if (keyCode == ALLEGRO_KEY_ENTER || keyCode == ALLEGRO_KEY_PAD_ENTER) {
        // Submit name
        if (!playerName.empty()) {
            // Add the new score record
            ScoreRecord newRecord;
            newRecord.name = playerName;
            newRecord.score = newScore;
            scoreRecords.push_back(newRecord);

            // Sort scores in descending order
            std::sort(scoreRecords.begin(), scoreRecords.end(),
                [](const ScoreRecord& a, const ScoreRecord& b) {
                    return a.score > b.score;
                });

            // Save scores
            SaveScores();

            // Return to showing scoreboard
            waitingForName = false;

            // Refresh the scoreboard display
            Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
        }
    }
    else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
        // Add letter (convert from keycode to character)
        if (playerName.length() < 10) {
            char c = 'A' + (keyCode - ALLEGRO_KEY_A);
            playerName += c;
        }
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Add number
        if (playerName.length() < 10) {
            char c = '0' + (keyCode - ALLEGRO_KEY_0);
            playerName += c;
        }
    }
    else if (keyCode == ALLEGRO_KEY_SPACE) {
        // Add space
        if (playerName.length() < 10 && !playerName.empty() && playerName.back() != ' ') {
            playerName += ' ';
        }
    }

    IScene::OnKeyDown(keyCode);
}

void ScoreboardScene::BackOnClick(int stage) {
    // Return to start scene
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void ScoreboardScene::AddScore(int lives, int money, int cheats) {
    cheatsUsed = cheats;
    newScore = CalculateScore(lives, money, cheatsUsed);

    // Check if the score is high enough to be on the scoreboard
    if (scoreRecords.size() < 10 || newScore > scoreRecords.back().score) {
        // Prompt for player name
        waitingForName = true;
        playerName = "";
    }
}

void ScoreboardScene::SaveScores() {
    std::ofstream outFile("Resource/scoreboard.txt");
    if (outFile.is_open()) {
        for (const auto& record : scoreRecords) {
            outFile << record.name << " " << record.score << std::endl;
        }
        outFile.close();
    }
}

void ScoreboardScene::LoadScores() {
    scoreRecords.clear();
    std::ifstream inFile("Resource/scoreboard.txt");
    if (inFile.is_open()) {
        std::string name;
        int score;

        // Read each line
        std::string line;
        while (std::getline(inFile, line)) {
            // Find the last space to separate name and score
            size_t lastSpace = line.find_last_of(" ");
            if (lastSpace != std::string::npos) {
                name = line.substr(0, lastSpace);
                try {
                    score = std::stoi(line.substr(lastSpace + 1));

                    ScoreRecord record;
                    record.name = name;
                    record.score = score;
                    scoreRecords.push_back(record);
                } catch (std::exception& e) {
                    // Error converting score, skip this record
                    continue;
                }
            }
        }

        inFile.close();

        // Sort scores in descending order
        std::sort(scoreRecords.begin(), scoreRecords.end(),
            [](const ScoreRecord& a, const ScoreRecord& b) {
                return a.score > b.score;
            });
    }
}