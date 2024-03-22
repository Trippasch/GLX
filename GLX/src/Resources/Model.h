#pragma once

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/GltfMaterial.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Resources/Mesh.h"
#include "Core/Log.h"
#include "Utils/assimp_glm_helpers.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

struct BoneInfo
{
    // id is index in finalBoneMatrices
    int id;
    // offset matrix transforms the vertex from model space to bone space
    glm::mat4 offset;
};

class Model
{
public:
    // model data 
    std::vector<Texture2D> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    VertexBuffer instancedArrowsVBO;
    size_t instancedArrowsSize;

    Model() {}
    // constructor, expects a filepath to a 3D model.
    Model(const std::string &path, bool gamma = false);

    // draws the model, and thus all its meshes
    void Draw(GLenum mode, const Shader &shader);

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCounter() { return m_BoneCounter; }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName, const aiScene *scene);

    Texture2D textureFromFile(const char *path, const std::string &directory, bool gamma = false);
    Texture2D textureFromEmbedded(const aiTexture* paiTexture, const std::string& directory, bool gamma = false);

    void setVertexBoneDataToDefault(Vertex& vertex);
    void setVertexBoneData(Vertex& vertex, int boneID, float weight);
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};