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

public:
    glm::vec3 m_Direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 1.0f;

    bool m_CastShadows = true;
    GLuint m_ShadowWidth = 1024;
    GLuint m_ShadowHeight = 1024;
    glm::vec3 m_DepthMapOrig = glm::vec3(0.0f);
    glm::mat4 m_DepthMapProjection;

    FrameBuffer m_DepthMapFBO;

    DirectionalLight(RendererLayer* renderer);
    ~DirectionalLight();

    void DirectionalLightProjectionMatrix();
    void RenderGUI(int i);
    void RenderDepthMapQuad();
};