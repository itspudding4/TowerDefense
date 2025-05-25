//
// Created by user on 5/24/2025.
//
#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/Bullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Turret.hpp"
#include "CircleShotTurret.hpp"
#include "Scene/PlayScene.hpp"

const int CircleShotTurret::Price = 150;

CircleShotTurret::CircleShotTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-fire.png", x, y, 450, Price, 2.5) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void CircleShotTurret::Update(float deltaTime) {
    Turret::Update(deltaTime);
}

void CircleShotTurret::CreateBullet() {
    for (int i = 0; i < 8; ++i) {
        float angle = i * ALLEGRO_PI * 2 / 8;
        Engine::Point dir = Engine::Point(cos(angle), sin(angle));

        getPlayScene()->BulletGroup->AddNewObject(
        new Bullet("play/bullet-8.png", 500, 10, Position + dir * 36, dir, 0, this)
        );
    }
    AudioHelper::PlayAudio("gun.wav");
}