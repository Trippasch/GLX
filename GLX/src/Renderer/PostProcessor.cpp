#include "Renderer/PostProcessor.h"

PostProcessor::PostProcessor(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    m_BloomSrcResolution = glm::vec2((float)m_Renderer->GetWidth(), (float)m_Renderer->GetHeight());

    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/postProcessingFS.glsl", nullptr, "post_proc");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/downsampleFS.glsl", nullptr, "downsample");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/upsampleFS.glsl", nullptr, "upsample");

    // Post Processing - Activate only one per group
    // Kernel effects
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
    // General Post Processing
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.bloom", m_UseBloom);
    ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.exposure", m_Exposure);
    ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.bloomStrength", m_BloomStrength);

    m_HDRFBO = FrameBuffer();
    m_HDRFBO.Bind();
    m_HDRFBO.TextureAttachment(2, 0, GL_TEXTURE_2D, GL_RGBA16F, m_Renderer->GetWidth(), m_Renderer->GetHeight());
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    m_BloomFBO = FrameBuffer();
    m_BloomFBO.Bind();
    m_BloomFBO.BloomAttachment(m_Renderer->GetWidth(), m_Renderer->GetHeight(), 6);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

PostProcessor::~PostProcessor()
{
    m_HDRFBO.Destroy();
    m_BloomFBO.Destroy();
}

void PostProcessor::ResizeBuffers(GLuint width, GLuint height)
{
    m_HDRFBO.Bind();
    m_HDRFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA16F, width, height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    m_BloomFBO.Bind();
    m_BloomFBO.ResizeBloomAttachment(width, height, 6);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

void PostProcessor::RenderPostProcessingGUI()
{
    ImGui::Begin("Post Processing");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Kernel Effects", m_Renderer->base_flags)) {
        if (ImGui::Checkbox("Blur", &m_UseBlur)) {
            m_UseEdge = false;
            m_UseRidge = false;
            m_UseSharpen = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
        }
        if (ImGui::Checkbox("Sharpen", &m_UseSharpen)) {
            m_UseEdge = false;
            m_UseRidge = false;
            m_UseBlur = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
        }
        if (ImGui::Checkbox("Ridge", &m_UseRidge)) {
            m_UseEdge = false;
            m_UseSharpen = false;
            m_UseBlur = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
        }
        if (ImGui::Checkbox("Edge", &m_UseEdge)) {
            m_UseRidge = false;
            m_UseSharpen = false;
            m_UseBlur = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("General Effects", m_Renderer->base_flags)) {
        if (ImGui::Checkbox("Greyscale", &m_UseGreyscale)) {
            m_UseInversion = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
        }
        if (ImGui::Checkbox("Inversion", &m_UseInversion)) {
            m_UseGreyscale = false;
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
            ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
        }
        if (ImGui::TreeNodeEx("Bloom", m_Renderer->base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseBloom)) {
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.bloom", m_UseBloom);
                if (m_UseBloom) {
                    if (ImGui::SliderFloat("Intensity", &m_BloomStrength, 0.0f, 0.5f, "%.2f")) {
                        ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.bloomStrength", m_BloomStrength);
                    }
                    if (ImGui::SliderFloat("Filter Radius", &m_BloomFilterRadius, 0.0f, 0.05f, "%.3f")) {
                        ResourceManager::GetShader("upsample").Use().SetFloat("filterRadius", m_BloomFilterRadius);
                    }
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("HDR Settings", m_Renderer->base_flags)) {
        if (ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.exposure", m_Exposure);
        }
    }

    ImGui::Separator();
    ImGui::End();
}

void PostProcessor::RenderPostProcessingQuad()
{
    glActiveTexture(GL_TEXTURE0);
    m_HDRFBO.BindTexture(GL_TEXTURE_2D, 0);
    if (m_UseBloom) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_BloomFBO.GetMipChain()[0].texture);
    }
    ResourceManager::GetShader("post_proc").Use();
    m_Renderer->RenderQuad(GL_TRIANGLES);
    Texture2D::UnBind();
}

void PostProcessor::RenderBloomTexture()
{
    m_BloomFBO.Bind();

    // Render Downsamples
    ResourceManager::GetShader("downsample").Use().SetVector2f("srcResolution", m_BloomSrcResolution);

    if (m_KarisAverageOnDownsample) {
        ResourceManager::GetShader("downsample").Use().SetInteger("mipLevel", 0);
    }

    glActiveTexture(GL_TEXTURE0);
    m_HDRFBO.BindTexture(GL_TEXTURE_2D, 1);

    // Progressively downsample through the mip chain
    for (size_t i = 0; i < m_BloomFBO.GetMipChain().size(); i++)
    {
        const BloomMip& mip = m_BloomFBO.GetMipChain()[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        m_Renderer->RenderQuad(GL_TRIANGLES);

        // Set current mip resolution as srcResolution for next iteration
        ResourceManager::GetShader("downsample").Use().SetVector2f("srcResolution", mip.size);
        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // Disable Karis average for consequent downsamples
        if (i == 0) {
            ResourceManager::GetShader("downsample").Use().SetInteger("mipLevel", 1);
        }
    }
    Texture2D::UnBind();

    // Render Upsamples
    ResourceManager::GetShader("upsample").Use().SetFloat("filterRadius", m_BloomFilterRadius);

    // Enable additive blending
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (size_t i = m_BloomFBO.GetMipChain().size() - 1; i > 0; i--)
    {
        const BloomMip& mip = m_BloomFBO.GetMipChain()[i];
        const BloomMip& nextMip = m_BloomFBO.GetMipChain()[i-1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        m_Renderer->RenderQuad(GL_TRIANGLES);
    }

    // Disable additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Texture2D::UnBind();

    FrameBuffer::UnBind();
    glViewport(0, 0, m_Renderer->GetWidth(), m_Renderer->GetHeight());
}