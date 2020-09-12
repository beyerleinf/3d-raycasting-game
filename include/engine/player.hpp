#pragma once

#include "utils/vector.hpp"

namespace ray3d
{

class Player
{
  public:
    Player(double posX, double posY, double dirX, double dirY);

    Vector position;
    Vector direction;
    Vector cameraPlane;
};

} // namespace ray3d
