#pragma once

#include <glad/glad.h>

class VertexBuffer
{
private:
    GLuint m_RendererID;
public:
    VertexBuffer() : m_RendererID(0) {}
    VertexBuffer(const GLvoid* data, GLuint size, GLenum mode);

    void LinkAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* data) const;
    void UnlinkAttrib(GLuint layout) const;
    void Destroy() const;
 private:
    void Bind() const;
    void UnBind() const;
};