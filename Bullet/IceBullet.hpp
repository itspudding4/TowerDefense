//
// Created by user on 5/24/2025.
//

#ifndef ICEBULLET_HPP
#define ICEBULLET_HPP

#include "Bullet.hpp"

class IceBullet : public Bullet {
public:
    explicit IceBullet(float x, float y, float rotation, Turret* parent);
    void OnExplode(Enemy* enemy) override;
};


#endif //ICEBULLET_HPP
