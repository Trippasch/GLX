#pragma once

#include <glad/glad.h>

class IndexBuffer
{
private:
    GLuint m_RendererID;
public:
    IndexBuffer() : m_RendererID(0) {}
    IndexBuffer(const GLvoid* data, GLuint size, GLenum mode);
    ~IndexBuffer();

    void Bind() const;
    void UnBind() const;
};
