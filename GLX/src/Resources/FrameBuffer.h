#pragma once

#include "Core/Log.h"

#include "Resources/Texture2D.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

struct BloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
};

class FrameBuffer
{
private:
    GLuint m_RendererID = 0;
    GLuint m_RenderBufferID = 0;

    std::vector<GLuint> textures;
    std::vector<BloomMip> mipChain;

public:
    FrameBuffer();

    void Bind() const;
    void Destroy() const;
    static void UnBind();
    static void CheckStatus();

    void BindTexture(GLenum target, GLuint index) const;

    void DepthMapAttachment(GLuint n, GLenum target, GLint inFormat, GLuint size, GLuint width, GLuint height);

    void TextureAttachment(GLuint n, GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height);
    void ResizeTextureAttachment(GLuint mode, GLenum target, GLint inFormat, GLuint width, GLuint height);
    void BloomAttachment(GLuint width, GLuint height, GLuint mipChainLength);
    void ResizeBloomAttachment(GLuint width, GLuint height, GLuint mipChainLength);
    void Blit(FrameBuffer fbo, GLuint width, GLuint height) const;
    void RenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height);
    void ResizeRenderBuffer(GLenum inFormat, GLuint width, GLuint height) const;
    void ResizeRenderBufferAttachment(GLboolean multisample, GLenum inFormat, GLuint width, GLuint height);

    inline GLuint &GetID() { return m_RendererID; }
    inline GLuint &GetRenderBufferID() { return m_RenderBufferID; }
    inline std::vector<GLuint> &GetTextureAttachments() { return textures; }
    inline std::vector<BloomMip> &GetMipChain() { return mipChain; }
};