#ifndef INVISIBLE_MODEL_H
#define INVISIBLE_MODEL_H
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"


class Shader;

class Model {
public:
    Model(std::string path) {
        loadModel(path);
    }

    void draw(Shader& shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<MESH::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};


#endif //INVISIBLE_MODEL_H