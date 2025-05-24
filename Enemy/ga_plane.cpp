#include <string>

#include "ga_plane.hpp"
#include "Scene/playScene.hpp"
// TODO HACKATHON-3 (1/3): You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
ga_planeEnemy::ga_planeEnemy(int x, int y) : Enemy("play/enemy-2.png", x, y, 30, 70, 10, 10 ) {
}

void ga_planeEnemy::UpdatePath(const std::vector<std::vector<int>>& /*mapDistance*/) {
    path.clear();
    path.push_back(PlayScene::EndGridPoint); //  Grid position, not pixel
}
