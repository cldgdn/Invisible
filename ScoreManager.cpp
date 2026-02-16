#include "ScoreManager.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include "globals.h"
#include "text/TextManager.h"

void ScoreManager::draw() {
    title->draw();
    backButton->draw();

    for (int i = 0; i < lines.size(); i++) {
        lines[i]->draw();
    }
}

bool ScoreManager::saveScore(ScoreEntry entry) {
    auto now = std::chrono::system_clock::now();
    entry.timestamp = std::chrono::system_clock::to_time_t(now);

    bool saved = false;
    if (scores.empty()) {
        scores.push_back(entry);
        saved = true;
    } else if (scores.size() < SCORING::MAX_SAVED_SCORES) {
        scores.push_back(entry);
        saved = true;
    } else {
        for (int i = 0; i < scores.size(); i++) {
            if (scores[i].score < entry.score) {
                scores.push_back(entry);
                saved = true;
                break;
            }
        }
    }

    if (!saved)
        return false;

    for (int i = 0; i < scores.size(); i++) {
        bool sorted = true;
        for (int j = 0; j < scores.size() - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                ScoreEntry temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
                sorted = false;
            }
        }
        if (sorted)
            break;
    }

    while (scores.size() > SCORING::MAX_SAVED_SCORES) {
        scores.erase(scores.end());
    }

    std::ofstream file("scores.inv", std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open scores.inv" << std::endl;
        return false;
    }

    for (ScoreEntry e : scores) {
        file.write(reinterpret_cast<char *>(&e), sizeof(e));
    }

    file.close();

    setupText();

    return true;
}

void ScoreManager::loadScores() {
    std::ifstream file("scores.inv", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open scores.inv" << std::endl;
        return;
    }

    ScoreEntry entry;
    while (file.read(reinterpret_cast<char*>(&entry), sizeof(ScoreEntry))) {
        scores.push_back(entry);
    }

    file.close();
}

ScoreManager::ScoreManager() {
    loadScores();
    TextManager &tm = TextManager::getInstance();

    backButton = tm.createText("Helvetica", 128, "!RETURN!");
    backButton->scale = 0.5f;
    backButton->position = {
        (LOGIC_SCREEN_WIDTH - backButton->getWidth() * backButton->scale) / 2,
        LOGIC_SCREEN_HEIGHT - (backButton->getHeight() * backButton->scale) - 10
    };
    title = tm.createText("Helvetica", 128, "LEADERBOARD");
    title->scale = 0.5f;
    title->position = {
        (LOGIC_SCREEN_WIDTH - title->getWidth() * title->scale) / 2,
        TILE_SIZE
    };

    lines.reserve(SCORING::MAX_SAVED_SCORES);
    for (int i = 0; i < SCORING::MAX_SAVED_SCORES; i++) {
        lines.push_back(tm.createText("Helvetica", 128, (i >= scores.size()) ? "" :makeText(scores[i])));
        lines[i]->scale = 0.25f;
        lines[i]->color = glm::vec4(0.7f, 0.0f, 0.0f, 0.7f);
        if (i == 0) {
            lines[i]->position = {
                (LOGIC_SCREEN_WIDTH - lines[i]->getWidth() * lines[i]->scale) / 2,
                title->position.y + title->getHeight() * title->scale + SCORE_MANAGER::LINES_OFFSET_FROM_TITLE
            };
        } else {
            lines[i]->position = {
                (LOGIC_SCREEN_WIDTH - lines[i]->getWidth() * lines[i]->scale) / 2,
                lines[i - 1]->position.y + lines[i - 1]->getHeight() * lines[i - 1]->scale + SCORE_MANAGER::LINES_SPACE
            };
        }
    }

}

ScoreManager::~ScoreManager() {
}

std::string ScoreManager::makeText(ScoreEntry entry) {
    std::ostringstream oss;

    oss << "Score: " << entry.score
        << " Time: " << entry.time / 60 << "." << entry.time % 60;
        //<< " date: " << std::put_time(std::localtime(&entry.timestamp), "%d/%m/%y");

    return oss.str();
}

void ScoreManager::setupText() {
    for (int i = 0; i < scores.size(); i++) {
        lines[i]->setText(makeText(scores[i]));
        if (i == 0) {
            lines[i]->position = {
                (LOGIC_SCREEN_WIDTH - lines[i]->getWidth() * lines[i]->scale) / 2,
                title->position.y + title->getHeight() * title->scale + SCORE_MANAGER::LINES_OFFSET_FROM_TITLE
            };
        } else {
            lines[i]->position = {
                (LOGIC_SCREEN_WIDTH - lines[i]->getWidth() * lines[i]->scale) / 2,
                lines[i - 1]->position.y + lines[i - 1]->getHeight() * lines[i - 1]->scale + SCORE_MANAGER::LINES_SPACE
            };
        }
    }
}
