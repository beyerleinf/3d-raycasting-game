#include "engine/player.hpp"

#include <stdio.h>

namespace ray3d
{

Player::Player(double posX, double posY, double dirX, double dirY)
{
    position = Vector(posX, posY);
    direction = Vector(dirX, dirY);
    cameraPlane = Vector(0, 0.66);
    direction.normalize();
}

} // namespace ray3d