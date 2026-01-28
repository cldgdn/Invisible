#ifndef INVISIBLE_ROOMEXIT_H
#define INVISIBLE_ROOMEXIT_H
#include <string>

#include "../Game.h"

enum ExitOrientation {
    HORIZONTAL,
    VERTICAL
};

class RoomExit : public Sprite {
public:
    std::string roomName;

    RoomExit(Game *game, std::string roomName, ExitOrientation orientation);
    ~RoomExit() = default;


};


#endif //INVISIBLE_ROOMEXIT_H