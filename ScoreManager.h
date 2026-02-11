#ifndef INVISIBLE_SCOREMANAGER_H
#define INVISIBLE_SCOREMANAGER_H
#include <ctime>
#include <vector>

#include "globals.h"
#include "GLFW/glfw3.h"
#include "text/TextObject.h"

namespace SCORING {
    constexpr int MAINTAIN_STEALTH = 20;    //add 10 points/s while staying entirely undetected
    constexpr int STAY_ALIVE = 10;           //add 5 points/s while staying alive
    constexpr int GET_SPOTTED = -50;        //remove 50 points whenever a guard is alerted
    constexpr int KILL = -50;              //remove 150 points whenever the player kills a guard
    constexpr int ROOM_BEAT = 150;          //add 150 points for beating a room
    constexpr int GAME_BEAT_BONUS = 150;    //add 150 extra points on top of ROOM_BEAT when beating the final room
    constexpr int MAX_SAVED_SCORES = 5;     //amount of scores that will be saved at one time
}

namespace SCORE_MANAGER {
    constexpr int LINES_SPACE = 4;
    constexpr int LINES_OFFSET_FROM_TITLE = TILE_SIZE;
}

struct ScoreEntry {
    int score, time;
    std::time_t timestamp;
};


class ScoreManager {
public:
    static ScoreManager& getInstance() {
        static ScoreManager instance;
        return instance;
    }

    void draw();

    bool saveScore(ScoreEntry entry);
    void loadScores();

private:
    std::vector<ScoreEntry> scores;
    std::vector<std::unique_ptr<TextObject>> lines;
    std::unique_ptr<TextObject> backButton, title;

    ScoreManager();
    ~ScoreManager();

    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    std::string makeText(ScoreEntry entry);
    void setupText();
};


#endif //INVISIBLE_SCOREMANAGER_H