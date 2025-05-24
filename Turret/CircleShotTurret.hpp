//
// Created by user on 5/24/2025.
//

#ifndef CIRCLESHOTTURRENT_HPP
#define CIRCLESHOTTURRENT_HPP

#include "Turret.hpp"

class CircleShotTurret : public Turret {
public:
    static const int Price;
    CircleShotTurret(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    bool shouldRotate() const override { return false; }
};
#endif //CIRCLESHOTTURRENT_HPP
