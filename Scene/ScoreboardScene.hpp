#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <string>
#include <vector>
#include <memory>
#include "Engine/IScene.hpp"

namespace Engine {
    class Image;
    class Label;
}

class ScoreboardScene final : public Engine::IScene {
private:
    struct ScoreRecord {
        std::string name;
        int score;
    };

    std::vector<ScoreRecord> scoreRecords;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    Engine::Image* background;
    std::vector<Engine::Label*> scoreLabels;
    std::vector<int> scoreLabelsId; // Added this vector that's used in Initialize()
    Engine::Label* titleLabel;
    Engine::Label* backLabel;
    std::string playerName;
    bool waitingForName;
    int newScore;
    int cheatsUsed;
    int nextBtnX = 1200, nextBtnY = 600, btnW = 120, btnH = 60;
    int prevBtnX = 300, prevBtnY = 600;
    int currentPage = 0;
    const int scoresPerPage = 5;
public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void RefreshScoreLabels();
    // Function to add new score
    void AddScore(int lives, int money, int cheats,const std::string& name);

    // Function to save scores to file
    void SaveScores();

    // Function to load scores from file
    void LoadScores();

    // Calculate score based on the formula
    int CalculateScore(int, int, int) ;

    void BackOnClick(int stage);
};

#endif // SCOREBOARDSCENE_HPP