//
// Created by clode on 08/11/2025.
//

#include "TileMap.h"

#include <bitset>

#include "../Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../globals.h"
#include "glm/gtc/type_ptr.hpp"

TileMap::TileMap(bool **solidMap, const std::string& path) : path(path) {
    remapTiles(solidMap);
    greedyMeshTiles();
}

TileMap::~TileMap() {
    for (Texture* texture : textures) {
        delete texture;
    }
}

void TileMap::remapTiles(bool **solidMap) {
    using namespace TILEMAP;
    unsigned char currNeighbours = 0;
    bool tileMatched;

    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            currNeighbours = mapNeighbours(solidMap, i, j);
            currNeighbours = (solidMap[i][j]) ? currNeighbours : ~currNeighbours;

            tileMatched = false;
            for (unsigned char k = 0; k < MAP_LENGTH; k++) {
                if (((currNeighbours & (MASK_TO_ID[k].mask)) == MASK_TO_ID[k].pattern)) {
                    tiles[i][j] = k + ((solidMap[i][j]) ? SOLID_BIT : 0);
                    tileMatched = true;
                    break;
                }
            }
            if (!tileMatched) {
                tiles[i][j] = SOLID_BIT;
                std::bitset<8> n(currNeighbours);
                std::cerr << "[TileMap]Tile (" << i << ", " << j << ") failed to match! ---- Neighbours: " << n << std::endl;
            }
        }
    }
}

unsigned char TileMap::mapNeighbours(bool **solidMap, int x, int y) {
    unsigned char neighbours = 0, currBit = 0b10000000;
    for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
            if (i == x && j == y) continue;

            if (i < 0 || i >= ROOM_HEIGHT || j < 0 || j >= ROOM_WIDTH || solidMap[i][j]) {
                    neighbours += currBit;
            }

            currBit >>= 1;
        }
    }

    return neighbours;
}

//create the texture objects by grouping each tile with the same value into the same Texture set to TILE by greedy meshing*
//on each tile, if MESHED_BIT is not set (therefore the tile wasn't placed in a mesh yet), check to the right all the way
//until the next tile has a different value (different texid, or same texid but already meshed), that will be the width of the mesh.
//After, try going down one and all the way to the right up to the last equal tile or until reaching the set width, whichever is first.
void TileMap::greedyMeshTiles() {
    int currWidth = 0, currHeight = 0, tempWidth = 0;

    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            if (!(tiles[i][j] & MESHED_BIT)) {
                for (currWidth = 1; j + currWidth < ROOM_WIDTH; currWidth++) {
                    if (tiles[i][j] != tiles[i][j + currWidth])
                        break;
                }

                for (currHeight = 1; i + currHeight < ROOM_HEIGHT; currHeight++) {
                    if (tiles[i][j] == tiles[i + currHeight][j]) {
                        for (tempWidth = 1; j + tempWidth < ROOM_WIDTH; tempWidth++) {
                            if (tempWidth == currWidth || tiles[i][j] != tiles[i + currHeight][j + tempWidth]) break;
                            tempWidth++;
                        }
                        if (tempWidth < currWidth)
                            break;
                    } else {
                        break;
                    }
                }

                for (int ii = 0; ii < currHeight; ii++) {
                    for (int jj = 0; jj < currWidth; jj++) {
                        tiles[i + ii][j + jj] = tiles[i + ii][j + jj] | MESHED_BIT;
                    }
                }

                std::string tileTex = path + ((tiles[i][j] & SOLID_BIT) ? "wall\\" : "floor\\") + "pixil-frame-" + std::to_string(tiles[i][j] & TEXTURE_ID_MASK) + ".png";
                Texture *t = new Texture(
                    tileTex,
                    TILE_SIZE * currWidth,
                    TILE_SIZE * currHeight,
                    Texture::TILE,
                    new Vec2(j * TILE_SIZE, i * TILE_SIZE)
                );
                textures.push_back(t);

                if (tiles[i][j] & SOLID_BIT) {
                    Collider *c = new Collider(
                        nullptr,
                        Vec2(j, i),
                        TILE_SIZE * currWidth,
                        TILE_SIZE * currHeight,
                        CLAYER_TILES,
                        ColliderType::SOLID,
                        true
                    );
                    colliders.push_back(c);
                }
            }
        }
    }
}

void TileMap::draw(bool debug) {
    for (Texture* texture : textures) {
        texture->draw({0, 0}, nullptr, debug);
    }
}


