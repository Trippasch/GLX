#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.h"
#include "Renderer/Texture2D.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    // int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    // float m_Weights[MAX_BONE_INFLUENCE];
};

// struct Texture
// {
//     GLuint id;
//     std::string type;
//     std::string path;
// };

class Mesh
{
public:
    // mesh Data
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture2D> textures;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures);

    // render the mesh
    void Draw(const Shader &shader) const;
    void Destroy() const;
    void Bind() const;
    void UnBind() const;

private:
    // render data
    VertexBuffer VBO;
    IndexBuffer EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};
