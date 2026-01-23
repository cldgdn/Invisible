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
    TANK,
    DESK
};

class Prop : public Sprite {
public:
    std::function<void(Game*)> action = nullptr;

    Prop(Game *game, PropType type, const std::string& postfix = "");
    ~Prop() override = default;

private:
    void makeTruck(const std::string& postfix);
    void makeBox(const std::string& postfix);
    void makeBoxSmall(const std::string& postfix);
    void makeTank(const std::string& postfix);
    void makeDesk(const std::string& postfix);
};


#endif //INVISIBLE_PROP_H