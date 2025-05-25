//
// Created by user on 5/24/2025.
//
// IceTurret.cpp
#include <allegro5/base.h>
#include "IceTurret.hpp"
#include "Bullet/IceBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Enemy/Enemy.hpp"
const int IceTurret::Price = 180;
#include "cmath"
IceTurret::IceTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-ice.png", x, y, 200, Price, 1.5) {
}

void IceTurret::CreateBullet() {
    Engine::Point diff = Target->Position - Position;
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Bullet goes from the tip of the tower
    Engine::Point normal = Engine::Point(normalized.y, -normalized.x);
    Engine::Point temp = Position + normalized * 30;
    getPlayScene()->BulletGroup->AddNewObject(new IceBullet(temp.x, temp.y, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}