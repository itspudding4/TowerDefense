#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <ctime>
#include <iomanip>
#include <sstream>

#include "ScoreboardScene.hpp"
#include "PlayScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

std::string GetCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* tmPtr = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tmPtr, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void ScoreboardScene::Initialize() {
    // Set default values
    playerName = "";
    waitingForName = false;
    newScore = 0;
    cheatsUsed = 0;
    scoreLabelsId.clear();
    scoreLabels.clear();
    currentPage = 0; // Ensure page starts at 0

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
    btn->SetOnClickCallback([this]() { BackOnClick(1); });
    AddNewControlObject(btn);

    backLabel = new Engine::Label("Back", "pirulen.ttf", 48, halfW, h - 50, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(backLabel);

    // Load scores first
    LoadScores();

    // Create headers
    int yPos = 150;
    Engine::Label* rankHeader = new Engine::Label("Rank", "pirulen.ttf", 30, halfW - 350, yPos, 255, 255, 255, 255, 0.0, 0.5);
    Engine::Label* nameHeader = new Engine::Label("Player", "pirulen.ttf", 30, halfW+30, yPos, 255, 255, 255, 255, 0.5, 0.5);
    Engine::Label* scoreHeader = new Engine::Label("Score", "pirulen.ttf", 30, halfW +450, yPos, 255, 255, 255, 255, 1.0, 0.5);
    Engine::Label* dateHeader = new Engine::Label("Time", "pirulen.ttf", 30, halfW -510, yPos, 255, 255, 255, 255, 1.0, 0.5);
    AddNewObject(rankHeader);
    AddNewObject(nameHeader);
    AddNewObject(scoreHeader);
    AddNewObject(dateHeader);
    scoreLabels = { rankHeader, nameHeader, scoreHeader,dateHeader };
    // Create navigation buttons only if needed


    // Display initial scores
    RefreshScoreLabels();

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

    // Draw name input overlay if needed
    if (waitingForName) {
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
        al_draw_filled_rectangle(0, 0, w, h, al_map_rgba(0, 0, 0, 180));
        al_draw_filled_rectangle(w/2 - 300, h/2 - 50, w/2 + 300, h/2 + 50, al_map_rgb(50, 50, 50));
        al_draw_rectangle(w/2 - 300, h/2 - 50, w/2 + 300, h/2 + 50, al_map_rgb(200, 200, 200), 2);
        Engine::Label namePrompt("Enter your name: " + playerName + "_", "pirulen.ttf", 28, w/2, h/2, 255, 255, 255, 255, 0.5, 0.5);
        namePrompt.Draw();
    }

    // Draw Prev button only on page 1
    if (currentPage == 1) {
        al_draw_filled_rectangle(prevBtnX, prevBtnY, prevBtnX + btnW, prevBtnY + btnH, al_map_rgb(100, 100, 100));
        Engine::Label prevLabel("Prev", "pirulen.ttf", 28, prevBtnX + btnW / 2, prevBtnY + btnH / 2, 255, 255, 255, 255, 0.5, 0.5);
        prevLabel.Draw();
    }
    // Draw Next button only on page 0
    if (currentPage == 0 && scoreRecords.size() > 5) {
        al_draw_filled_rectangle(nextBtnX, nextBtnY, nextBtnX + btnW, nextBtnY + btnH, al_map_rgb(100, 100, 100));
        Engine::Label nextLabel("Next", "pirulen.ttf", 28, nextBtnX + btnW / 2, nextBtnY + btnH / 2, 255, 255, 255, 255, 0.5, 0.5);
        nextLabel.Draw();
    }

}

void ScoreboardScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (scoreRecords.empty()) return;

    // Check Next button (when on page 0)
    if (currentPage == 0 && mx >= nextBtnX && mx <= nextBtnX + btnW &&
        my >= nextBtnY && my <= nextBtnY + btnH) {
        if (scoreRecords.size() > 5) {
            currentPage = 1;
            RefreshScoreLabels();
        }
        }
    // Check Prev button (when on page 1)
    else if (currentPage == 1 && mx >= prevBtnX && mx <= prevBtnX + btnW &&
             my >= prevBtnY && my <= prevBtnY + btnH) {
        currentPage = 0;
        RefreshScoreLabels();
             }
}
// C++
void ScoreboardScene::RefreshScoreLabels() {
    // Remove old score labels first (excluding header labels)
    for (size_t i = 4; i < scoreLabels.size(); i++) {
        Engine::Label* label = scoreLabels[i];
        auto it = std::find_if(objects.begin(), objects.end(),
            [label](const std::pair<bool, Engine::IObject*>& obj) {
                return obj.second == label;
            });
        if (it != objects.end()) {
            RemoveObject(it);
        }
    }

    // Clear score labels (keeping header labels)
    scoreLabels.resize(4);

    // Calculate range for current page
    int start = currentPage * 5;
    int end = std::min(start + 5, static_cast<int>(scoreRecords.size()));

    // Early return if invalid page: reset to first page.
    if (start >= static_cast<int>(scoreRecords.size())) {
        currentPage = 0;
        return;
    }

    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int yPos = 200;

    // Add new score labels
    for (int i = start; i < end; ++i) {
        Engine::Label* rankLabel = new Engine::Label(
            std::to_string(i + 1) + ".", "pirulen.ttf", 24,
            halfW - 300, yPos, 255, 255, 255, 255, 0.0, 0.5
        );
        Engine::Label* nameLabel = new Engine::Label(
            scoreRecords[i].name, "pirulen.ttf", 24,
            halfW+30, yPos, 255, 255, 255, 255, 0.5, 0.5
        );
        Engine::Label* scoreLabel = new Engine::Label(
            std::to_string(scoreRecords[i].score), "pirulen.ttf", 24,
            halfW +430, yPos, 255, 255, 255, 255, 1.0, 0.5
        );
        Engine::Label* dateLabel = new Engine::Label(
            scoreRecords[i].dateTime, "pirulen.ttf", 20,
            halfW -450, yPos, 255, 255, 255, 255, 1.0, 0.5
        );

        AddNewObject(rankLabel);
        AddNewObject(nameLabel);
        AddNewObject(scoreLabel);
        AddNewObject(dateLabel);

        scoreLabels.push_back(rankLabel);
        scoreLabels.push_back(nameLabel);
        scoreLabels.push_back(scoreLabel);
        scoreLabels.push_back(dateLabel);
        yPos += 40;
    }
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
int ScoreboardScene::CalculateScore(int lives, int money, int cheatsUsed) {
    int lifeScore = lives * 1000;
    int moneyScore = money;
    int cheatPenalty = cheatsUsed * 5000;

    std::cout << "Lives: " << lives << " (Score: " << lifeScore << ")" << std::endl;
    std::cout << "Money: " << money << std::endl;
    std::cout << "Cheats: " << cheatsUsed << " (Penalty: " << cheatPenalty << ")" << std::endl;

    int finalScore = lifeScore + moneyScore - cheatPenalty;
    return std::max(finalScore, 0);
}
void ScoreboardScene::AddScore(int lives, int money, int cheats,const std::string& name) {
    LoadScores();
    cheatsUsed = cheats;
    newScore = CalculateScore(lives, money, cheatsUsed);
    playerName = name;

    ScoreRecord newRecord;
    newRecord.name = name;
    newRecord.score = newScore;
    newRecord.dateTime = GetCurrentDateTime();//for time
    scoreRecords.push_back(newRecord);


    std::sort(scoreRecords.begin(), scoreRecords.end(),
        [](const ScoreRecord& a, const ScoreRecord& b) {
            return a.score > b.score;
        });

    SaveScores();
    waitingForName = false;
}

void ScoreboardScene::SaveScores() {
    std::ofstream outFile("Resource/scoreboard.txt");
    if (outFile.is_open() && !scoreRecords.empty()) {
        for (const auto& record : scoreRecords) {
            outFile << record.name << "|" << record.score << "|" << record.dateTime << std::endl;
        }
        outFile.close();
    }
}

void ScoreboardScene::LoadScores() {
    scoreRecords.clear();
    std::ifstream inFile("Resource/scoreboard.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            size_t firstSep = line.find('|');
            size_t secondSep = line.find('|', firstSep + 1);
            if (firstSep != std::string::npos && secondSep != std::string::npos) {
                std::string name = line.substr(0, firstSep);
                int score = std::stoi(line.substr(firstSep + 1, secondSep - firstSep - 1));
                std::string dateTime = line.substr(secondSep + 1);
                ScoreRecord record;
                record.name = name;
                record.score = score;
                record.dateTime = dateTime;
                scoreRecords.push_back(record);
            }
        }
        inFile.close();
    }
    std::sort(scoreRecords.begin(), scoreRecords.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
        return a.score > b.score;
    });
}
