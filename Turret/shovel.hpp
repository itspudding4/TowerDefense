//
// Created by user on 5/25/2025.
//
#include "turret.hpp"
#ifndef SHOVEL_HPP
#define SHOVEL_HPP
class Shovel : public Turret {
private:
    ALLEGRO_BITMAP* img;
protected:
    void CreateBullet() override;
public:
    Shovel(float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif //SHOVEL_HPP
