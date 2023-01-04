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

    void BindTexture(GLenum target, GLuint index) const;

    void TextureAttachment(GLuint n, GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height);
    void ResizeTextureAttachment(GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height);
    void Blit(FrameBuffer fbo, GLuint width, GLuint height) const;
    void RenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height);
    void ResizeRenderBuffer(GLenum inFormat, GLuint width, GLuint height) const;
    void ResizeRenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height);

    inline GLuint &GetID() { return m_RendererID; }
    inline GLuint &GetRenderBufferID() { return m_RenderBufferID; }
    inline std::vector<GLuint> &GetTextureAttachments() { return textures; }
};