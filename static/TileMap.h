//
// Created by clode on 08/11/2025.
//

#ifndef INVISIBLE_TILEMAP_H
#define INVISIBLE_TILEMAP_H
#include <vector>

#include "Texture.h"
#include "../Shader.h"
#include "glm/glm.hpp"

namespace TILEMAP {
    struct Map {
        unsigned char mask, pattern;
    };

    static const int MAP_LENGTH = 47;
    static const Map MASK_TO_ID[] = {
        {0b01011010, 0b00000000},
        {0b01011010, 0b00010000},
        {0b01011010, 0b01000000},
        {0b01011010, 0b00001000},
        {0b01011010, 0b00000010},
        {0b01011110, 0b00010110},
        {0b01011110, 0b00010010},
        {0b11011010, 0b11010000},
        {0b11011010, 0b01010000},
        {0b01111010, 0b01101000},
        {0b01111010, 0b01001000},
        {0b01011011, 0b00001011},
        {0b01011011, 0b00001010},
        {0b01011111, 0b00011111},
        {0b01011111, 0b00011011},
        {0b01011111, 0b00011110},
        {0b01011111, 0b00011010},
        {0b11011110, 0b11010110},
        {0b11011110, 0b01010110},
        {0b11011110, 0b11010010},
        {0b11011110, 0b01010010},
        {0b11111010, 0b11111000},
        {0b11111010, 0b11011000},
        {0b11111010, 0b01111000},
        {0b11111010, 0b01011000},
        {0b01111011, 0b01101011},
        {0b01111011, 0b01001011},
        {0b01111011, 0b01101010},
        {0b01111011, 0b01001010},
        {0b01011010, 0b00011000},
        {0b01011010, 0b01000010},
        {0b11111111, 0b11111111},
        {0b11111111, 0b11011111},
        {0b11111111, 0b11111110},
        {0b11111111, 0b11111011},
        {0b11111111, 0b01111111},
        {0b11111111, 0b01011111},
        {0b11111111, 0b11011110},
        {0b11111111, 0b11111010},
        {0b11111111, 0b01111011},
        {0b11111111, 0b01011110},
        {0b11111111, 0b11011010},
        {0b11111111, 0b01111010},
        {0b11111111, 0b01011011},
        {0b11111111, 0b01011010},
        {0b11111111, 0b01111110},
        {0b11111111, 0b11011011}
    };
}


class TileMap {
public:
    static constexpr unsigned int ROOM_WIDTH = 16;
    static constexpr unsigned int ROOM_HEIGHT = 8;
    static constexpr unsigned int TILE_SIZE = 16;
    static constexpr unsigned char TEXTURE_ID_MASK = 0b00111111;
    static constexpr unsigned char SOLID_BIT = 0b01000000;
    static constexpr unsigned char MESHED_BIT = 0b10000000;

    unsigned char tiles[ROOM_HEIGHT][ROOM_WIDTH];
    std::vector<Texture*> textures;
    const std::string& path;

    TileMap(bool **solidMap, const std::string& path);
    ~TileMap();

    void draw(bool debug);

private:
    void remapTiles(bool **solidMap);
    unsigned char mapNeighbours(bool **solidMap, int x, int y);
    void greedyMeshTiles();
};


#endif //INVISIBLE_TILEMAP_H