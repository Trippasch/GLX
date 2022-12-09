#include "Renderer/FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_RendererID);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void FrameBuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Destroy() const
{
    glDeleteRenderbuffers(1, &m_RenderBufferID);
    glDeleteFramebuffers(1, &m_RendererID);
    for (size_t i = 0; i < textures.size(); i++)
        glDeleteTextures(1, &textures[i]);
    glDeleteTextures(1, &envCubemap);
}

void FrameBuffer::CheckStatus()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GL_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}

void FrameBuffer::BindTexture(GLuint index) const
{
    glBindTexture(GL_TEXTURE_2D, textures[index]);
}

void FrameBuffer::BindCubemap() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
}

void FrameBuffer::TextureAttachment(GLuint n, GLenum mode, GLint inFormat, GLuint width, GLuint height)
{
    GLuint *tex = new GLuint[n];
    glGenTextures(n, tex);

    for (size_t i = 0; i < n; i++) {
        glBindTexture(mode, tex[i]);

        if (mode == GL_TEXTURE_2D) {
            glTexImage2D(mode, 0, inFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
            glTexImage2DMultisample(mode, 4, inFormat, width, height, GL_TRUE);

        glBindTexture(mode, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mode, tex[i], 0);
    }

    textures.clear();
    for (GLuint i = 0; i < n; i++)
        textures.push_back(tex[i]);

    delete [] tex;
}

void FrameBuffer::CubemapAttachment(GLuint width, GLuint height)
{
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (GLuint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void FrameBuffer::ResizeTextureAttachment(GLenum mode, GLint inFormat, GLuint width, GLuint height)
{
    for (size_t i = 0; i < textures.size(); i++) {
        glBindTexture(mode, textures[i]);

        if (mode == GL_TEXTURE_2D) {
            glTexImage2D(mode, 0, inFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
            glTexImage2DMultisample(mode, 4, inFormat, width, height, GL_TRUE);

        glBindTexture(mode, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mode, textures[i], 0);
    }
}

void FrameBuffer::Blit(FrameBuffer fbo, GLuint width, GLuint height) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.m_RendererID);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::RenderBufferAttachment(GLboolean multisample, GLuint width, GLuint height)
{
    glGenRenderbuffers(1, &m_RenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

    if (multisample)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
}

void FrameBuffer::ResizeRenderBufferAttachment(GLboolean multisample, GLuint width, GLuint height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

    if (multisample)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
}