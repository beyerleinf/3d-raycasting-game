#include "engine/world.hpp"
#include <stdio.h>

namespace ray3d
{

World::World()
{
}

World::~World()
{
    map.clear();
}

void World::load()
{
    for (int x = 0; x < 48; x++)
    {
        for (int y = 0; y < 48; y++)
        {
            if (x == 0 || x == 47 || y == 0 || y == 47)
            {
                map[x][y] = WorldTile(WorldTileType::Wall1);
            }
            else
            {
                map[x][y] = WorldTile(WorldTileType::Floor);
            }
        }
    }

    map[20][10] = WorldTile(WorldTileType::Wall1);
}

WorldTile *World::getTile(int x, int y)
{
    WorldTile *result = nullptr;

    if (checkCoordinates(x, y))
    {
        result = &map[x][y];
    }

    return result;
}

bool World::isValidPosition(int x, int y)
{
    bool result = false;

    if (checkCoordinates(x, y))
    {
        result = map[x][y].type == WorldTileType::Floor;
    }

    return result;
}

bool World::checkCoordinates(int x, int y)
{
    bool result = false;

    if (x >= 0 && x < map.size())
    {
        if (y >= 0 && y < map[x].size())
        {
            result = true;
        }
    }

    return result;
}

} // namespace ray3d
