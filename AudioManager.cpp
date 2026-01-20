//
// Created by clode on 19/01/2026.
//

#include "AudioManager.h"

bool AudioManager::loadSound(const std::string &name, const std::string &path) {
    auto sound = std::make_unique<SoLoud::Wav>();
    if (sound->load(path.c_str()) != SoLoud::SO_NO_ERROR) {
        return false;
    }

    sounds[name] = std::move(sound);

    return true;
}

unsigned int AudioManager::playSound(const std::string &name, float volume, bool loop) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        unsigned int handle = soloud.play(*it->second, volume);

        if (loop)
            soloud.setLooping(handle, true);

        handles[name] = handle;

        return handle;
    }

    return 0;
}

void AudioManager::stopSound(const std::string &name) {
    auto it = handles.find(name);

    if (it != handles.end()) {
        soloud.stop(it->second);
        handles.erase(it);
    }
}

void AudioManager::setVolume(const std::string &name, float volume) {
    auto it = handles.find(name);

    if (it != handles.end()) {
        soloud.setVolume(it->second, volume);
    }
}

void AudioManager::stopAllSounds() {
    soloud.stopAll();
    handles.clear();
}
