#pragma once

#include <glad/glad.h>

class UniformBuffer
{
private:
    GLuint ID;

public:
    UniformBuffer() : ID(0) {}
    UniformBuffer(GLuint index, GLuint offset, GLuint size, GLuint mode);

    void FillBuffer(const GLvoid* data, GLuint offset, GLuint size) const;
    void Destroy() const;

    GLuint& GetID() { return ID; }

private:
    void Bind() const;
    void UnBind() const;
};