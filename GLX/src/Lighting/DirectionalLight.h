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

public:
    glm::vec3 m_Direction = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 1.0f;

    bool m_CastShadows = true;
    GLuint m_DepthMapResolution = 4096;

    std::vector<float> m_ShadowCascadeLevels;

    FrameBuffer m_DepthMapFBO;

    DirectionalLight(RendererLayer* renderer);
    ~DirectionalLight();

    void RenderGUI(int i);
    void RenderDepthMapQuad();
    void RenderDebugGUI();

    std::vector<glm::mat4> GetLightSpaceMatrices();

private:
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projView);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
    glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
};