//
// Created by user on 5/25/2025.
//

#include "shovel.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/base.h>
#include <cmath>
#include <string>

Shovel::Shovel(float x, float y) : Turret("play/tower-base.png", "play/shovel.png", x, y, 0,  0, 0) {
}

void Shovel::Update(float deltaTime) {

}
void Shovel::CreateBullet() {

}
void Shovel::Draw() const {
    Turret::Draw();
    // Optionally, draw a highlight or effect
}