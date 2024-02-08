#pragma once

#include "Resources/VertexBuffer.h"
#include <memory>

class VertexArray
{
private:
    GLuint m_RendererID;

public:
    VertexArray();
    ~VertexArray();

    // Links a vbo Attribute such as a position or color to the VAO
    void LinkAttrib(std::unique_ptr<VertexBuffer>& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const;
    // Binds the VAO
    void Bind() const;
    // Unbinds the VAO
    void UnBind() const;
};