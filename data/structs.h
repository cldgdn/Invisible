//
// Created by clode on 07/11/2025.
//

#ifndef INVISIBLE_STRUCTS_H
#define INVISIBLE_STRUCTS_H

struct Vec2 {
    float x, y;

    Vec2 operator+(const Vec2 & vec2) const;
    Vec2 operator-(const Vec2 & vec2) const;
};

inline Vec2 Vec2::operator+(const Vec2 &vec2) const {
    return Vec2(this->x + vec2.x, this->y + vec2.y);
}

inline Vec2 Vec2::operator-(const Vec2 &vec2) const {
    return Vec2(this->x - vec2.x, this->y - vec2.y);
}

struct Vec3 {
    float x, y, z;
};



struct UVinfo {
    Vec2 texCoord;
    float width, height;
};

#endif //INVISIBLE_STRUCTS_H