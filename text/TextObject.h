//
// Created by clode on 08/02/2026.
//

#ifndef INVISIBLE_TEXTOBJECT_H
#define INVISIBLE_TEXTOBJECT_H
#include <memory>

#include "../data/structs.h"
#include "glm/vec4.hpp"


class Font;

class TextObject {
public:
    Vec2 position = {0.0f, 0.0f};
    float scale = 1.0f;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    TextObject(std::shared_ptr<Font> font, const std::string& text);
    ~TextObject();

    TextObject(const TextObject&) = delete;
    TextObject& operator=(const TextObject&) = delete;

    void setText(const std::string& text);
    const std::string& getText() const;

    float getWidth();
    float getHeight();

    void draw();
private:
    std::shared_ptr<Font> font;
    std::string text;

    unsigned int VAO, VBO;
    unsigned int vertexCount;

    float width = -1, height = -1;

    void rebuildMesh();
};


#endif //INVISIBLE_TEXTOBJECT_H