//
// Created by clode on 21/01/2026.
//

#ifndef INVISIBLE_PROP_H
#define INVISIBLE_PROP_H
#include "Sprite.h"

enum PropType {
    TRUCK,
    BOX,
    BOX_SMALL,
    TANK
};

class Prop : public Sprite {
public:
    std::function<void(Game*)> action = nullptr;

    Prop(Game *game, PropType type);
    ~Prop() override = default;

private:
    void makeTruck();
    void makeBox();
    void makeBoxSmall();
    void makeTank();
};


#endif //INVISIBLE_PROP_H