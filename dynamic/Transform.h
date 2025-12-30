//
// Created by clode on 07/11/2025.
//

#ifndef INVISIBLE_TRANSFORM_H
#define INVISIBLE_TRANSFORM_H

#include "../data/structs.h"

class Transform {
public:
    Vec2 position;
    Vec2 rotation;
    Vec2 scale;

    bool translatePending;

    Transform();

    void translate2d(const Vec2& translation);
    void confirmTranslate2d();
    void undoTranslate2d();
private:
    Vec2 currentTranslation2d;
};

#endif //INVISIBLE_TRANSFORM_H