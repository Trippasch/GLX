#pragma once

#include "Resources/ResourceManager.h"
#include "Renderer/RendererLayer.h"
#include "Resources/FrameBuffer.h"

class RendererLayer;

class PostProcessor
{
private:
    RendererLayer* m_Renderer;

    bool m_UseGreyscale = false;
    bool m_UseInversion = false;
    bool m_UseRidge = false;
    bool m_UseEdge = false;
    bool m_UseSharpen = false;
    bool m_UseBlur = false;
    float m_Exposure = 0.5f;
    float m_BloomFilterRadius = 0.005f;
    float m_BloomStrength = 0.04f;
    bool m_KarisAverageOnDownsample = true;
    bool m_UseBloom = true;

    glm::vec2 m_BloomSrcResolution;

    FrameBuffer m_BloomFBO;
    FrameBuffer m_HDRFBO;

public:
    PostProcessor(RendererLayer* renderer);
    ~PostProcessor();

    FrameBuffer& GetHDRFBO() { return m_HDRFBO; }
    FrameBuffer& GetBloomFBO() { return m_BloomFBO; }
    const bool& GetUseBloom() const { return m_UseBloom; }

    void ResizeBuffers(GLuint width, GLuint height);
    void RenderBloomTexture();
    void RenderPostProcessingQuad();
    void RenderPostProcessingGUI();
};