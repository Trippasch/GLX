#include "Renderer/VertexArray.h"

// #include "Utils/helper_gl.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexArray::LinkAttrib(std::unique_ptr<VertexBuffer>& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const
{
    vbo->Bind();
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    vbo->UnBind();
    // SDK_CHECK_ERROR_GL();
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexArray::UnBind() const
{
    glBindVertexArray(0);
    // SDK_CHECK_ERROR_GL();
}
