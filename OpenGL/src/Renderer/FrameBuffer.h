#pragma once

#include <glad/glad.h>

class FrameBuffer
{
private:
    GLuint m_RendererID;

public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void UnBind() const;
};