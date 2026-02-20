//
// Created by clode on 16/02/2026.
//

#ifndef INVISIBLE_MESH_H
#define INVISIBLE_MESH_H
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Shader;

namespace MESH {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };
}

class Mesh {
public:
    std::vector<MESH::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MESH::Texture> textures;

    Mesh(std::vector<MESH::Vertex> vertices, std::vector<unsigned int> indices, std::vector<MESH::Texture> textures);

    void draw(Shader& shader);
private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};


#endif //INVISIBLE_MESH_H