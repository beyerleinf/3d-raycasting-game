#pragma once

#include <cmath>

namespace ray3d
{
class Vector
{
  public:
    Vector() : Vector(0, 0){};
    Vector(double positionX, double positionY);

    double distanceTo(Vector *target);
    void normalize();
    void set(double positionX, double positionY);

    double x;
    double y;
};
} // namespace ray3d
