#pragma once

#include <glad/glad.h>

class VertexBuffer
{
private:
    GLuint m_RendererID;
public:
    VertexBuffer() : m_RendererID(0) {}
    VertexBuffer(const GLvoid* data, GLuint size, GLenum mode);
    ~VertexBuffer();

    void Bind() const;
    void UnBind() const;
};