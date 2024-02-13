#pragma once

#include "Resources/ResourceManager.h"
#include "Resources/FrameBuffer.h"
#include "Renderer/RendererLayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RendererLayer;

class PointLight
{
private:
    RendererLayer* m_Renderer;

public:
    glm::vec3 m_Position = glm::vec3(0.0f, 4.0f, 4.0f);
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 20.0f;

    bool m_CastShadows = true;
    GLuint m_ShadowWidth = 1024;
    GLuint m_ShadowHeight = 1024;
    glm::mat4 m_PointLightProjection;

    FrameBuffer m_DepthCubeMapFBO;

    PointLight(RendererLayer* renderer);
    ~PointLight();

    void PointLightProjectionMatrix();
    void RenderMesh();
};