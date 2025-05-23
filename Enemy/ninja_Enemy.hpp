//
// Created by user on 5/23/2025.

#ifndef NINJA_ENEMY_HPP
#define NINJA_ENEMY_HPP

#include "Enemy.hpp"

class ninja_Enemy : public Enemy {
public:
    ninja_Enemy(int x, int y);
    void Hit(float damage) override;       // Handle invisibility when hit
    void Update(float deltaTime) override; // Count down invisibility time
    bool IsVisible() const override { return !invisible; }
    void Draw() const override;

private:
    bool invisible = false;
    float invisibilityTimer = 0.0f;
};

#endif // NINJA_ENEMY_HPP
