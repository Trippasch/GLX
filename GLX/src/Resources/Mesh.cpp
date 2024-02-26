#include "Resources/Mesh.h"

#include <iostream>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture2D> &textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
    VBO = VertexBuffer(&vertices[0], vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);
    EBO = IndexBuffer(&indices[0], indices.size() * sizeof(GLuint), GL_STATIC_DRAW);
}

void Mesh::Bind() const
{
    EBO.Bind();
    // vertex positions
    VBO.LinkAttrib(0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    // vertex normals
    VBO.LinkAttrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    VBO.LinkAttrib(2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    VBO.LinkAttrib(3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    VBO.LinkAttrib(4, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    // VBO.LinkAttrib(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    // VBO.LinkAttrib(6, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
}

void Mesh::UnBind() const
{
    VBO.UnlinkAttrib(0);
    VBO.UnlinkAttrib(1);
    VBO.UnlinkAttrib(2);
    VBO.UnlinkAttrib(3);
    VBO.UnlinkAttrib(4);
    // VBO.UnlinkAttrib(5);
    // VBO.UnlinkAttrib(6);
    EBO.UnBind();
}

// render the mesh
void Mesh::Draw(GLenum mode, const Shader &shader) const
{
    for (unsigned int i = 0; i < textures.size(); i++)
        textures[i].Bind(GL_TEXTURE_2D, i+3);

    // draw mesh
    Bind();
    glDrawElements(mode, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    UnBind();

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Destroy() const
{
    VBO.Destroy();
    EBO.Destroy();
}