#include "Renderer/IndexBuffer.h"

// #include "helper_gl.h"

IndexBuffer::IndexBuffer(const GLvoid* data, GLuint size, GLenum mode)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, mode);
    // SDK_CHECK_ERROR_GL();
}

void IndexBuffer::Destroy() const
{
    glDeleteBuffers(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void IndexBuffer::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // SDK_CHECK_ERROR_GL();
}
