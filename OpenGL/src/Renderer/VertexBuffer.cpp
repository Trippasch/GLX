#include "Renderer/VertexBuffer.h"

// #include "helper_gl.h"

VertexBuffer::VertexBuffer(const GLvoid* data, GLuint size, GLenum mode)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
    // SDK_CHECK_ERROR_GL();
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
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