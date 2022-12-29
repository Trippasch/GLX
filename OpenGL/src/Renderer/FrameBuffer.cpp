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

void FrameBuffer::TextureAttachment(GLuint n, GLenum mode, GLint inFormat, GLuint width, GLuint height)
{
    GLuint *tex = new GLuint[n];
    glGenTextures(n, tex);

    for (size_t i = 0; i < n; i++) {
        glBindTexture(mode, tex[i]);

        if (mode == GL_TEXTURE_2D) {
            glTexImage2D(mode, 0, inFormat, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else if (mode == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(mode, 4, inFormat, width, height, GL_TRUE);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glBindTexture(mode, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mode, tex[i], 0);
    }

    textures.clear();
    for (GLuint i = 0; i < n; i++)
        textures.push_back(tex[i]);

    delete [] tex;
}

void FrameBuffer::ResizeTextureAttachment(GLenum mode, GLint inFormat, GLuint width, GLuint height)
{
    for (size_t i = 0; i < textures.size(); i++) {
        glBindTexture(mode, textures[i]);

        if (mode == GL_TEXTURE_2D) {
            glTexImage2D(mode, 0, inFormat, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else if (mode == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(mode, 4, inFormat, width, height, GL_TRUE);
            glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glBindTexture(mode, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mode, textures[i], 0);
    }
}

void FrameBuffer::Blit(FrameBuffer fbo, GLuint width, GLuint height) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.m_RendererID);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::RenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height)
{
    glGenRenderbuffers(1, &m_RenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

    if (multisample)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, inFormat, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, inFormat, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (inFormat == GL_RGB16F)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RenderBufferID);
    else if (inFormat == GL_DEPTH24_STENCIL8)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
    else if (inFormat == GL_DEPTH_COMPONENT)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
}

void FrameBuffer::ResizeRenderBuffer(GLenum inFormat, GLuint width, GLuint height) const
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, inFormat, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBuffer::ResizeRenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

    if (multisample)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, inFormat, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, inFormat, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (inFormat == GL_RGB16F)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RenderBufferID);
    else if (inFormat == GL_DEPTH24_STENCIL8)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
    else if (inFormat == GL_DEPTH_COMPONENT)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
}