#pragma once
struct MoveResult {
    int result; //0 - no collision, 1 - wall collision, 2 - floor/ceil collision?
    float fraction;
};

static class Collision
{
public:
    static MoveResult Move(float posX, float posY, float radius, float moveX, float moveY, int mapWidth, int mapHeight, int* tiles, int tileRadius);
};

