//
// Created by clode on 08/02/2026.
//

#ifndef INVISIBLE_FONT_H
#define INVISIBLE_FONT_H
#include <string>

#include <ft2build.h>
#include <unordered_map>

#include "Glyph.h"

#include FT_FREETYPE_H


class Font {
public:
    std::string fontPath;
    unsigned int pixelSize;
    FT_Face face;

    Font(const std::string& fontPath, unsigned int pixelSize, FT_Library ft);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    const Glyph& getGlyph(char c);
private:
    std::unordered_map<char, Glyph> glyphs;

    void loadGlyph(char c);
};


#endif //INVISIBLE_FONT_H