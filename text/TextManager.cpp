//
// Created by clode on 08/02/2026.
//

#include "TextManager.h"

#include <iostream>

TextManager::TextManager()
    : ft(nullptr)
{
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "Error initializing FreeType library" << std::endl;
    }
}

TextManager::~TextManager()
{
    unloadAllFonts();

    if (ft)
    {
        FT_Done_FreeType(ft);
        ft = nullptr;
    }
}

std::shared_ptr<Font> TextManager::loadFont(
    const std::string& fontPath,
    const std::string& fontName,
    unsigned int pixelSize
)
{
    // Check if font name already exists
    auto& sizeMap = fonts[fontName];

    auto it = sizeMap.find(pixelSize);
    if (it != sizeMap.end())
    {
        // Font already loaded, return existing instance
        return it->second;
    }

    // Create and store new font
    auto font = std::make_shared<Font>(fontPath, pixelSize, ft);
    sizeMap.emplace(pixelSize, font);

    return font;
}

void TextManager::unloadFont(
    const std::string& fontName,
    unsigned int pixelSize
)
{
    auto fontIt = fonts.find(fontName);
    if (fontIt == fonts.end())
        return;

    auto& sizeMap = fontIt->second;
    sizeMap.erase(pixelSize);

    // If no sizes remain, remove font name entry
    if (sizeMap.empty())
    {
        fonts.erase(fontIt);
    }
}

void TextManager::unloadAllFonts()
{
    fonts.clear();
}

std::unique_ptr<TextObject> TextManager::createText(
    const std::string& fontName,
    unsigned int pixelSize,
    const std::string& text
)
{
    auto fontIt = fonts.find(fontName);
    if (fontIt == fonts.end())
    {
        throw std::runtime_error(
            "Font not loaded: " + fontName
        );
    }

    auto& sizeMap = fontIt->second;
    auto sizeIt = sizeMap.find(pixelSize);

    if (sizeIt == sizeMap.end())
    {
        throw std::runtime_error(
            "Font size not loaded: " + fontName +
            " (" + std::to_string(pixelSize) + ")"
        );
    }

    return std::make_unique<TextObject>(sizeIt->second, text);
}
