//
// Created by clode on 08/02/2026.
//

#ifndef INVISIBLE_TEXTMANAGER_H
#define INVISIBLE_TEXTMANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Font.h"
#include "TextObject.h"

class TextManager {
public:
    static TextManager& getInstance() {
        static TextManager instance;
        return instance;
    }

    std::shared_ptr<Font> loadFont(
        const std::string& fontPath,
        const std::string& fontName,
        unsigned int pixelSize
    );

    void unloadFont(
        const std::string& fontPath,
        unsigned int pixelSize
    );

    void unloadAllFonts();

    std::unique_ptr<TextObject> createText(
        const std::string& fontName,
        unsigned int pixelSize,
        const std::string& text
    );
private:
    using FontSizeMap = std::unordered_map<unsigned int, std::shared_ptr<Font>>;

    FT_Library ft;
    std::unordered_map<std::string, FontSizeMap> fonts;

    TextManager();
    ~TextManager();

    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;
};


#endif //INVISIBLE_TEXTMANAGER_H