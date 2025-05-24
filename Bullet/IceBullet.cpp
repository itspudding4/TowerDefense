// IceBullet.cpp
#include "IceBullet.hpp"
#include "Enemy/Enemy.hpp"
#include <cmath>

IceBullet::IceBullet(float x, float y, float rotation, Turret* parent)
    : Bullet("play/icebullet.png", 500, 0.5,//i set 0.5 cuz its because it is iceexxxd
             Engine::Point(x, y),
             Engine::Point(std::cos(rotation), std::sin(rotation)),
             rotation - ALLEGRO_PI / 2, parent) {
}

void IceBullet::OnExplode(Enemy* enemy) {
    enemy->Freeze(2.0f); // Freeze enemy for 2 seconds
}