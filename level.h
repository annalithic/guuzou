#pragma once
struct Level
{
    int sizeX;
    int sizeY;
    int* tiles;

    Level(const char* levelfile);

    Level() = default;
};

