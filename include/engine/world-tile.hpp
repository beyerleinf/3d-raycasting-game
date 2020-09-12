#pragma once

namespace ray3d
{

enum WorldTileType
{
    Floor,
    Wall1,
    Wall2
};

class WorldTile
{
  public:
    WorldTile() : WorldTile(WorldTileType::Floor){};
    WorldTile(WorldTileType t);

    WorldTileType type;
};

} // namespace ray3d