#include "Resources/Mesh.h"

#include <iostream>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::unordered_map<std::string, std::vector<Texture2D>> &textures)
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
    VBO.LinkAttribI(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
    // weights
    VBO.LinkAttrib(6, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Weights));
}

void Mesh::UnBind() const
{
    VBO.UnlinkAttrib(0);
    VBO.UnlinkAttrib(1);
    VBO.UnlinkAttrib(2);
    VBO.UnlinkAttrib(3);
    VBO.UnlinkAttrib(4);
    VBO.UnlinkAttrib(5);
    VBO.UnlinkAttrib(6);
    EBO.UnBind();
}

// render the mesh
void Mesh::Draw(GLenum mode, const Shader &shader) const
{
    // Bind each texture to its corresponding texture unit
    if (textures.count("texture_albedo") > 0) {
        for (auto&& tex : textures.at("texture_albedo"))
            tex.Bind(GL_TEXTURE_2D, 3);
    }

    if (textures.count("texture_normal") > 0) {
        for (auto&& tex : textures.at("texture_normal"))
            tex.Bind(GL_TEXTURE_2D, 4);
    }

    if (textures.count("texture_metallic") > 0) {
        for (auto&& tex : textures.at("texture_metallic"))
            tex.Bind(GL_TEXTURE_2D, 5);
    }

    if (textures.count("texture_roughness") > 0) {
        for (auto&& tex : textures.at("texture_roughness"))
            tex.Bind(GL_TEXTURE_2D, 6);
    }

    if (textures.count("texture_specular") > 0) {
        for (auto&& tex : textures.at("texture_specular"))
            tex.Bind(GL_TEXTURE_2D, 7);
    }

    if (textures.count("texture_ao") > 0) {
        for (auto&& tex : textures.at("texture_ao"))
            tex.Bind(GL_TEXTURE_2D, 8);
    }

    if (textures.count("texture_emissive") > 0) {
        for (auto&& tex : textures.at("texture_emissive"))
            tex.Bind(GL_TEXTURE_2D, 9);
    }

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