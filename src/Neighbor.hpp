#pragma once

#include <string>

enum Direction { EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST, NORTH, NORTH_EST };

/**
 * Structure représentant un voisin avec sa direction
 */
struct Neighbor {
    int cellId;
    Direction direction;

    Neighbor(int id, Direction dir) : cellId(id), direction(dir) {}
};