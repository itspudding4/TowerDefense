//
// Created by user on 5/24/2025.
//

#ifndef ICETURRET_HPP
#define ICETURRET_HPP
#include "Turret.hpp"

class IceTurret : public Turret {
protected:
    void CreateBullet() override;
public:
    IceTurret(float x, float y);
    static const int Price;
};
#endif //ICETURRET_HPP
