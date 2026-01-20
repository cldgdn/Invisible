//
// Created by clode on 19/01/2026.
//

#ifndef INVISIBLE_AUDIOMANAGER_H
#define INVISIBLE_AUDIOMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "soloud.h"
#include "soloud_wav.h"


class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    bool loadSound(const std::string& name, const std::string& path);
    unsigned int playSound(const std::string& name, float volume, bool loop);
    void stopSound(const std::string& name);
    void setVolume(const std::string& name, float volume);
    void stopAllSounds();

private:
    SoLoud::Soloud soloud;
    std::unordered_map<std::string, unsigned int> handles;
    std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> sounds;

    AudioManager() { soloud.init(); }
    ~AudioManager() { soloud.deinit(); }

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
};


#endif //INVISIBLE_AUDIOMANAGER_H