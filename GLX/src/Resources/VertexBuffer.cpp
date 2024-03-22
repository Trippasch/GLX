#include "Resources/VertexBuffer.h"

// #include "helper_gl.h"

VertexBuffer::VertexBuffer(const GLvoid* data, GLuint size, GLenum mode)
{
    glGenBuffers(1, &m_RendererID);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
    UnBind();
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::Destroy() const
{
    glDeleteBuffers(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::LinkAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* offset) const
{
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, offset);
    UnBind();
}

void VertexBuffer::LinkAttribI(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* offset) const
{
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribIPointer(index, size, type, stride, offset);
    UnBind();
}

void VertexBuffer::UnlinkAttrib(GLuint layout) const
{
    glDisableVertexAttribArray(layout);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // SDK_CHECK_ERROR_GL();
}