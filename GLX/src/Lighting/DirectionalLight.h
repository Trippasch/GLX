#pragma once

#include "Resources/ResourceManager.h"
#include "Resources/FrameBuffer.h"
#include "Renderer/RendererLayer.h"

#include <glm/glm.hpp>

class RendererLayer;

class DirectionalLight
{
private:
    RendererLayer* m_Renderer;

    GLint m_DebugDepthMapLayer = 2;

    glm::vec3 m_Direction = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 1.0f;
    bool m_CastShadows = true;
    GLuint m_DepthMapResolution = 1024;

    std::vector<float> m_ShadowCascadeLevels;

    FrameBuffer m_DepthMapFBO;

public:
    DirectionalLight(RendererLayer* renderer);
    ~DirectionalLight();

    void RenderGUI(int i);
    void RenderDepthMapQuad();
    void RenderDebugGUI();

    void SetDirection(const glm::vec3& direction) { m_Direction = direction; }
    void SetColor(const glm::vec3& color) { m_Color = color; }
    void SetIntensity(const float intensity) { m_Intensity = intensity; }
    void SetCastShadows(const bool castShadows) { m_CastShadows = castShadows; }
    void SetDepthMapResolution(const GLuint resolution) { m_DepthMapResolution = resolution; }
    void SetShadowCascadeLevels(const std::vector<float>& shadowCascadeLevels) { m_ShadowCascadeLevels = shadowCascadeLevels; }
    void SetDepthMapFBO(const FrameBuffer& depthMapFBO) { m_DepthMapFBO = depthMapFBO; }

    const glm::vec3& GetDirection() const { return m_Direction; }
    const glm::vec3& GetColor() const { return m_Color; }
    const float& GetIntensity() const { return m_Intensity; }
    const bool& GetCastShadows() const { return m_CastShadows; }
    const GLuint& GetDepthMapResolution() const { return m_DepthMapResolution; }
    const std::vector<float>& GetShadowCascadeLevels() const { return m_ShadowCascadeLevels; }
    const FrameBuffer& GetDepthMapFBO() const { return m_DepthMapFBO; }

    std::vector<glm::mat4> GetLightSpaceMatrices();

private:
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projView);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
    glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
};