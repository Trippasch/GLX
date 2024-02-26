#include "Resources/FrameBuffer.h"

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
    if (m_RenderBufferID != 0)
        glDeleteRenderbuffers(1, &m_RenderBufferID);
    if (m_RendererID != 0)
        glDeleteFramebuffers(1, &m_RendererID);
    for (size_t i = 0; i < textures.size(); i++) {
        if (textures[i] != 0)
            glDeleteTextures(1, &textures[i]);
    }
    for (size_t i = 0; i < translucentTextures.size(); i++) {
        if (translucentTextures[i] != 0)
            glDeleteTextures(1, &translucentTextures[i]);
    }
    for (size_t i = 0; i < mipChain.size(); i++) {
        if (mipChain[i].texture != 0)
            glDeleteTextures(1, &mipChain[i].texture);
    }
}

void FrameBuffer::CheckStatus()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GL_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}

void FrameBuffer::BindTexture(GLenum target, GLuint index) const
{
    glBindTexture(target, textures[index]);
}

void FrameBuffer::BindTranslucentTexture(GLenum target, GLuint index) const
{
    glBindTexture(target, translucentTextures[index]);
}

void FrameBuffer::BloomAttachment(GLuint width, GLuint height, GLuint mipChainLength)
{
    glm::vec2 mipSize((float)width, (float)height);
    glm::ivec2 mipIntSize((int)width, (int)height);

    for (unsigned int i = 0; i < mipChainLength; i++) {
        BloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // we are downscaling an HDR color buffer, so we need a float texture format
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mipChain.emplace_back(mip);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].texture, 0);

    // setup attachments
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
}

void FrameBuffer::ResizeBloomAttachment(GLuint width, GLuint height, GLuint mipChainLength)
{
    glm::vec2 mipSize((float)width, (float)height);
    glm::ivec2 mipIntSize((int)width, (int)height);

    mipChain.clear();

    for (unsigned int i = 0; i < mipChainLength; i++) {
        BloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // we are downscaling an HDR color buffer, so we need a float texture format
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mipChain.emplace_back(mip);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].texture, 0);

    // setup attachments
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
}

void FrameBuffer::DepthMapAttachment(GLuint n, GLenum target, GLint inFormat, GLuint size, GLuint width, GLuint height)
{
    GLuint *tex = new GLuint[n];
    glGenTextures(n, tex);

    for (size_t i = 0; i < n; i++) {
        glBindTexture(target, tex[i]);

        glTexImage3D(target, 0, inFormat, width, height, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindTexture(target, 0);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    textures.clear();
    for (GLuint i = 0; i < n; i++)
        textures.push_back(tex[i]);

    delete [] tex;
}

void FrameBuffer::TranslucentAttachments(GLenum target, GLuint width, GLuint height)
{
    GLuint *tex = new GLuint[2];
    glGenTextures(2, tex);

    for (size_t i = 0; i < 2; i++) {
        glBindTexture(target, tex[i]);

        if (i == 0) {
            // accum texture
            if (target == GL_TEXTURE_2D) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, target, tex[i], 0);
        }
        else if (i == 1) {
            // reveal texture
            if (target == GL_TEXTURE_2D) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_R8, width, height, GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, target, tex[i], 0);
        }

        glBindTexture(target, 0);
    }

    translucentTextures.clear();
    for (GLuint i = 0; i < 2; i++)
        translucentTextures.push_back(tex[i]);

    delete [] tex;
}

void FrameBuffer::ResizeTranslucentAttachments(GLenum target, GLuint width, GLuint height)
{
    for (size_t i = 0; i < translucentTextures.size(); i++) {
        glBindTexture(target, translucentTextures[i]);

        if (i == 0) {
            // accum texture
            if (target == GL_TEXTURE_2D) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, target, translucentTextures[i], 0);
        }
        else if (i == 1) {
            // reveal texture
            if (target == GL_TEXTURE_2D) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_R8, width, height, GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, target, translucentTextures[i], 0);
        }

        glBindTexture(target, 0);
    }
}

void FrameBuffer::TextureAttachment(GLuint n, GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height)
{
    GLuint *tex = new GLuint[n];
    glGenTextures(n, tex);

    for (size_t i = 0; i < n; i++) {
        glBindTexture(target, tex[i]);

        if (target == GL_TEXTURE_2D) {
            if (mode == 0) {
                glTexImage2D(target, 0, inFormat, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (mode == 1) {
                glTexImage2D(target, 0, inFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            }
        }
        else if (target == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(target, 4, inFormat, width, height, GL_TRUE);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else if (target == GL_TEXTURE_CUBE_MAP) {
            if (mode == 2) {
                for (GLuint i = 0; i < 6; i++)
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, inFormat, width, height, 0, inFormat, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
        }

        if (mode == 0) {
            glBindTexture(target, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, tex[i], 0);
        }
        else if (mode == 1) {
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
            glBindTexture(target, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, tex[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else if (mode == 2) {
            glBindTexture(target, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }

    textures.clear();
    for (GLuint i = 0; i < n; i++)
        textures.push_back(tex[i]);

    delete [] tex;
}

void FrameBuffer::ResizeTextureAttachment(GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height)
{
    for (size_t i = 0; i < textures.size(); i++) {
        glBindTexture(target, textures[i]);

        if (target == GL_TEXTURE_2D) {
            if (mode == 0) {
                glTexImage2D(target, 0, inFormat, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else if (mode == 1) {
                glTexImage2D(target, 0, inFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            }
        }
        else if (target == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(target, 4, inFormat, width, height, GL_TRUE);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else if (target == GL_TEXTURE_CUBE_MAP) {
            if (mode == 2) {
                for (GLuint i = 0; i < 6; i++)
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, inFormat, width, height, 0, inFormat, GL_FLOAT, NULL);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
        }

        if (mode == 0) {
            glBindTexture(target, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, textures[i], 0);
        }
        else if (mode == 1) {
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
            glBindTexture(target, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, textures[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else if (mode == 2) {
            glBindTexture(target, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
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