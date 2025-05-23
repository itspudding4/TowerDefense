
// ninja_Enemy.cpp

#include "ninja_Enemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Group.hpp"
#include "Engine/AudioHelper.hpp"
#include "Turret/Turret.hpp"
#include "Bullet/Bullet.hpp"
ninja_Enemy::ninja_Enemy(int x, int y)
    : Enemy("play/ninja-removebg.png", x, y, 10, 50, 5, 20) {
}

void ninja_Enemy::Hit(float damage) {
    if (invisible) return; // Can't be hit while invisible

    hp -= damage;
    if (hp <= 0) {
        OnExplode();
        for (auto& it : lockedTurrets)
            it->Target = nullptr;
        for (auto& it : lockedBullets)
            it->Target = nullptr;
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    } else {
        invisible = true;
        invisibilityTimer = 1.5f; // Become invisible for 1.5 seconds
    }
}

void ninja_Enemy::Update(float deltaTime) {
    Enemy::Update(deltaTime); // Keep the usual movement/pathing logic

    if (invisible) {
        invisibilityTimer -= deltaTime;
        if (invisibilityTimer <= 0.0f) {
            invisible = false;
        }
    }
}

void ninja_Enemy::Draw() const {
    if (!invisible)
        Enemy::Draw();
}