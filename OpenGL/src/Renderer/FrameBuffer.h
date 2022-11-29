#pragma once

#include "Core/Log.h"

#include <glad/glad.h>

#include "Renderer/Texture2D.h"

class FrameBuffer
{
private:
    GLuint m_RendererID;
    GLuint m_RenderBufferID;

    std::vector<GLuint> textures;

public:
    FrameBuffer();

    void Bind() const;
    void Destroy() const;
    static void UnBind();
    static void CheckStatus();

    void BindTexture(GLuint index);

    void TextureAttachment(GLuint n, GLenum mode, GLint inFormat, GLuint width, GLuint height);
    void ResizeTextureAttachment(GLenum mode, GLint inFormat, GLuint width, GLuint height);
    void Blit(FrameBuffer fbo, GLuint width, GLuint height) const;
    void RenderBufferAttachment(GLboolean multisample, GLuint width, GLuint height);
    void ResizeRenderBufferAttachment(GLboolean multisample, GLuint width, GLuint height);

    inline std::vector<GLuint> GetTextureAttachments() const { return textures; }
};