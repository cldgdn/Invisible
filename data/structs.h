//
// Created by clode on 07/11/2025.
//

#ifndef INVISIBLE_STRUCTS_H
#define INVISIBLE_STRUCTS_H

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
};

struct Mesh {
    float *vertices;
    unsigned int *indices;
    unsigned int vertexCount;
    unsigned int indexCount;
};

struct UVinfo {
    Vec2 texCoord;
    float width, height;
};

#endif //INVISIBLE_STRUCTS_H