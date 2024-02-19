#include "Resources/UniformBuffer.h"

UniformBuffer::UniformBuffer(GLuint index, GLuint offset, GLuint size, GLenum mode)
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, mode);

    glBindBufferBase(GL_UNIFORM_BUFFER, index, ID);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, index, ID, offset, size);
}

void UniformBuffer::FillBuffer(const GLvoid* data, GLuint offset, GLuint size) const
{
    Bind();
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    UnBind();
}

void UniformBuffer::Destroy() const
{
    glDeleteBuffers(1, &ID);
}

void UniformBuffer::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UniformBuffer::UnBind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}