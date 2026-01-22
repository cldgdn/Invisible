//
// Created by clode on 08/11/2025.
//

#include "TileMap.h"

#include <bitset>
#include <ranges>

#include "../Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../globals.h"
#include "glm/gtc/type_ptr.hpp"

TileMap::TileMap(bool **solidMap, const std::string& path, bool simpleFloor) : path(path), simpleFloor(simpleFloor) {
    remapTiles(solidMap);
    greedyMeshTiles();
}

TileMap::~TileMap() {
    for (Texture* texture : textures) {
        delete texture;
    }

    for (Collider* collider : colliders) {
        delete collider;
    }
}

/*
 * starting from a boolean value, determine the exact id for each tile
 */
void TileMap::remapTiles(bool **solidMap) {
    using namespace TILEMAP;
    unsigned char currNeighbours = 0;
    bool tileMatched;

    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            currNeighbours = mapNeighbours(solidMap, i, j);
            currNeighbours = (solidMap[i][j]) ? currNeighbours : ~currNeighbours; //if current tile is not solid, negate neighbors to have 1s where other non-solid neighbors are.

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

/*
 * generates a byte where each bit (ordered 01234567) is set to 1 if the corresponding tile is solid or out of bounds.
 * mapping (T is the current tile):
 * 012
 * 3T4
 * 567
 */
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

/*
 * create the texture objects by grouping each tile with the same value into the same Texture set to TILE by greedy meshing
 * on each tile, if MESHED_BIT is not set (therefore the tile wasn't placed in a mesh yet), check to the right all the way
 * until the next tile has a different value (different texid, or same texid but already meshed), that will be the width of the mesh.
 * After, try going down one and all the way to the right up to the last equal tile or until reaching the set width, whichever is first.
 * Once a group is formed, it gets assigned a texture and (if solid) a collider is also created.
 */

void TileMap::greedyMeshTiles() {
    int currWidth = 0, currHeight = 0, tempWidth = 0;

    if (simpleFloor) {
        std::string tileTex = path + "floor.png";
        Texture *floor = new Texture(
            tileTex,
            TILE_SIZE * ROOM_WIDTH, TILE_SIZE * ROOM_HEIGHT,
            Texture::TILE, new Vec2{0, 0}
        );
        textures.push_back(floor);
    }

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

                if (!simpleFloor || tiles[i][j] & SOLID_BIT) {
                    std::string tileTex = path + ((tiles[i][j] & SOLID_BIT) ? "wall/" : "floor/") + "pixil-frame-" + std::to_string(tiles[i][j] & TEXTURE_ID_MASK) + ".png";
                    Texture *t = new Texture(
                        tileTex,
                        TILE_SIZE * currWidth,
                        TILE_SIZE * currHeight,
                        Texture::TILE,
                        new Vec2(j * TILE_SIZE, i * TILE_SIZE)
                    );
                    textures.push_back(t);
                }

                if (tiles[i][j] & SOLID_BIT) {
                    Collider *c = new Collider(
                        nullptr,
                        Vec2(j * TILE_SIZE, i * TILE_SIZE),
                        TILE_SIZE * currWidth,
                        TILE_SIZE * currHeight,
                        CLAYER_TILES,
                        0,
                        ColliderType::SOLID,
                        true
                    );
                    colliders.push_back(c);
                }
            }
        }
    }

    //create colliders to wall in the entire map

    Collider *topWall = new Collider(
        nullptr,
        Vec2(0.0f - TILE_SIZE, 0.0f - TILE_SIZE),
        (TILE_SIZE + 2) * ROOM_WIDTH,
        TILE_SIZE,
        CLAYER_TILES,
        0,
        ColliderType::SOLID,
        true
    );
    Collider *bottomWall = new Collider(
        nullptr,
        Vec2(0.0f - TILE_SIZE, TILE_SIZE * ROOM_HEIGHT),
        (TILE_SIZE + 2) * ROOM_WIDTH,
        TILE_SIZE,
        CLAYER_TILES,
        0,
        ColliderType::SOLID,
        true
    );
    Collider *leftWall = new Collider(
        nullptr,
        Vec2(0.0f - TILE_SIZE, 0),
        TILE_SIZE,
        TILE_SIZE * ROOM_HEIGHT,
        CLAYER_TILES,
        0,
        ColliderType::SOLID,
        true
    );
    Collider *rightWall = new Collider(
        nullptr,
        Vec2(TILE_SIZE * ROOM_WIDTH, 0),
        TILE_SIZE,
        TILE_SIZE * ROOM_HEIGHT,
        CLAYER_TILES,
        0,
        ColliderType::SOLID,
        true
    );

    colliders.push_back(topWall);
    colliders.push_back(bottomWall);
    colliders.push_back(leftWall);
    colliders.push_back(rightWall);
}

void TileMap::draw(bool debug) {
    for (Texture* texture : textures) {
        texture->draw({0, 0}, nullptr, debug);
    }
}


