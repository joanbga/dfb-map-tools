#pragma once

#include <string>
#include "Direction.hpp"

/**
 * Structure représentant un voisin avec sa direction
 */
struct Neighbor
{
    int cellId;
    Direction direction;

    Neighbor(int id, Direction dir) : cellId(id), direction(dir) {}
};