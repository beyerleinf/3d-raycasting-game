#pragma once

#include <vector>

#include "engine/world-tile.hpp"

namespace ray3d
{

class World
{
  public:
    World();
    ~World();

    void load();
    WorldTile *getTile(int x, int y);
    bool isValidPosition(int x, int y);

  private:
    bool checkCoordinates(int x, int y);

    std::vector<std::vector<WorldTile>> map = std::vector<std::vector<WorldTile>>(48, std::vector<WorldTile>(48));
};

} // namespace ray3d