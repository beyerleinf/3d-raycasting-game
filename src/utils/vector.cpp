#include "utils/vector.hpp"

namespace ray3d
{
Vector::Vector(double positionX, double positionY)
{
    x = positionX;
    y = positionY;
}

void Vector::normalize()
{
    const double magnitude = sqrt(pow(x, 2) + pow(y, 2));
    x = x / magnitude;
    y = y / magnitude;
}

void Vector::set(double positionX, double positionY)
{
    x = positionX;
    y = positionY;
}

double Vector::distanceTo(Vector *target)
{
    return sqrt(pow((x - target->x), 2) + pow((y - target->y), 2));
}

} // namespace ray3d