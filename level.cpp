#include "level.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

void ReadRoom(int* room, int roomSizeX, int roomSizeY, std::ifstream& stream) {
    char* line = new char[roomSizeX + 1];
    for (int y = 0; y < roomSizeY; y++) {
        stream >> line;
        for (int x = 0; x < roomSizeX; x++) {
            room[x + y * roomSizeX] = line[x] == '0' ? 0 : line[x] == 'D' ? 2 : 1;
        }
    }
    delete(line);
}

void SetRoom(int* room, int roomX, int roomY, int* tiles, int sizeX, int roomSizeX, int roomSizeY) {
    for (int y = 0; y < roomSizeY; y++) {
        for (int x = 0; x < roomSizeX; x++) {
            tiles[(roomX + 1) * roomSizeX + x + ((roomY + 1) * roomSizeY + y) * sizeX] = room[x + y * roomSizeX];
        }
    }
}

Level::Level(const char* levelfile)
{
    const int roomSizeX = 10;
    const int roomSizeY = 8;

    int entrance     [roomSizeX * roomSizeY];
    int entrance_down[roomSizeX * roomSizeY];
    int exit         [roomSizeX * roomSizeY];
    int exit_up      [roomSizeX * roomSizeY];
    int side         [roomSizeX * roomSizeY];
    int path         [roomSizeX * roomSizeY];
    int path_up      [roomSizeX * roomSizeY];
    int path_down    [roomSizeX * roomSizeY];
    int path_vert    [roomSizeX * roomSizeY];

    std::ifstream stream{ levelfile };
    std::string line;
    while (stream >> line) {
        if (line[0] == '.') {
            if (line == ".entrance") ReadRoom(entrance, roomSizeX, roomSizeY, stream);
            else if (line == ".entrance_down") ReadRoom(entrance_down, roomSizeX, roomSizeY, stream);
            else if (line == ".exit") ReadRoom(exit, roomSizeX, roomSizeY, stream);
            else if (line == ".exit_up") ReadRoom(exit_up, roomSizeX, roomSizeY, stream);
            else if (line == ".side") ReadRoom(side, roomSizeX, roomSizeY, stream);
            else if (line == ".path") ReadRoom(path, roomSizeX, roomSizeY, stream);
            else if (line == ".path_up") ReadRoom(path_up, roomSizeX, roomSizeY, stream);
            else if (line == ".path_down") ReadRoom(path_down, roomSizeX, roomSizeY, stream);
            else if (line == ".path_vert") ReadRoom(path_vert, roomSizeX, roomSizeY, stream);
        }
    }


    const int roomsX = 4;
    const int roomsY = 4;

    sizeX = roomSizeX * (roomsX + 2); //one room of surrounding bedrock
    sizeY = roomSizeY * (roomsY + 2);
    tiles = new int[sizeX * sizeY];
    for (int i = 0; i < sizeX * sizeY; i++) tiles[i] = 1;

    for(int y = 0; y < roomsY; y++)
        for(int x = 0; x < roomsX; x++)
            SetRoom(side, x, y, tiles, sizeX, roomSizeX, roomSizeY);

    std::srand(std::time(nullptr));
    int start = rand() % 4;


    for (int y = 0; y < roomsY; y++) {
        int end = rand() % 4;
        if (start == end) {
            int* room = y == 0 ? entrance_down : y == roomsY - 1 ? exit_up : path_vert;
            SetRoom(room, start, y, tiles, sizeX, roomSizeX, roomSizeY);
        }
        else {
            int* startRoom = y == 0 ? entrance : path_up;
            int* endRoom = y == roomsY - 1 ? exit : path_down;
            SetRoom(startRoom, start, y, tiles, sizeX, roomSizeX, roomSizeY);
            SetRoom(endRoom, end, y, tiles, sizeX, roomSizeX, roomSizeY);
            for (int x = start + 1; x < end; x++) SetRoom(path, x, y, tiles, sizeX, roomSizeX, roomSizeY);
            for (int x = start - 1; x > end; x--) SetRoom(path, x, y, tiles, sizeX, roomSizeX, roomSizeY);
            start = end;
        }

    }

    //set last door to exit (maybe include room type in setroom)
    for (int i = sizeX * sizeY - 1; i >= 0; i--) {
        if (tiles[i] == 2) {
            tiles[i] = 3;
            break;
        }
    }
}