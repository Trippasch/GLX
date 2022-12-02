#include "Renderer/Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture2D> textures)
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
void Mesh::Draw(const Shader &shader) const
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].Type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }

    // draw mesh
    Bind();
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    UnBind();

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Destroy() const
{
    VBO.Destroy();
    EBO.Destroy();
}