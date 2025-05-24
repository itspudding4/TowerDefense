#ifndef GA_PLANEENEMY_HPP
#define GA_PLANEENEMY_HPP
#include "Enemy.hpp"

class ga_planeEnemy : public Enemy {
public:
    ga_planeEnemy(int x, int y);
    void UpdatePath(const std::vector<std::vector<int>>& mapDistance) override;
};
#endif
