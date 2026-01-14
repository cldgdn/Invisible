//
// Created by clode on 18/11/2025.
//

#ifndef INVISIBLE_GLOBALS_H
#define INVISIBLE_GLOBALS_H

extern double deltaTime;
extern unsigned int SCREEN_WIDTH, SCREEN_HEIGHT, LOGIC_SCREEN_WIDTH, LOGIC_SCREEN_HEIGHT;

constexpr unsigned int ROOM_WIDTH = 16;
constexpr unsigned int ROOM_HEIGHT = 8;
constexpr unsigned int TILE_SIZE = 16;
constexpr double FIXED_DT = 1.0 / 60.0;

constexpr unsigned short int
    CLAYER_TILES = 0b0000000000000001,
    CLAYER_PLAYER = 0b0000000000000010,
    CLAYER_ENEMY = 0b0000000000000100,
    CLAYER_INTERACTION = 0b0000000000010000,
    CLAYER_SOLID_PROPS = 0b0000000000100000;

#endif //INVISIBLE_GLOBALS_H