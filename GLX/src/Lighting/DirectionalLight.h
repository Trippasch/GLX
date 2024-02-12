#pragma once

#include "Resources/ResourceManager.h"
#include "Resources/FrameBuffer.h"

#include <glm/glm.hpp>

class DirectionalLight
{
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

    DirectionalLight()
    {
        ResourceManager::LoadShader("assets/shaders/depthMapVS.glsl", "assets/shaders/depthMapFS.glsl", nullptr, "depth_map");
        ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/depthQuadFS.glsl", nullptr, "depth_quad");

        m_DepthMapFBO.Bind();
        m_DepthMapFBO.TextureAttachment(1, 1, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();
    }

    ~DirectionalLight()
    {
        m_DepthMapFBO.Destroy();
    }

    void DirectionalLightProjectionMatrix()
    {
        float boxLimit = 20.0f;
        float minX = -boxLimit;
        float maxX = boxLimit;
        float minY = -boxLimit;
        float maxY = boxLimit;
        float minZ = -boxLimit;
        float maxZ = boxLimit;

        glm::vec3 center = glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);

        float width = maxX - minX;
        float height = maxY - minY;
        float depth = maxZ - minZ;

        float nearDistance = glm::length(center - m_DepthMapOrig) - depth / 2.0f;
        float farDistance = glm::length(center - m_DepthMapOrig) + depth / 2.0f;

        float left = center.x - width / 2.0f - m_DepthMapOrig.x;
        float right = center.x + width / 2.0f - m_DepthMapOrig.x;
        float bottom = center.y - height / 2.0f - m_DepthMapOrig.y;
        float top = center.y + height / 2.0f - m_DepthMapOrig.y;

        m_DepthMapProjection = glm::ortho(left, right, bottom, top, nearDistance, farDistance);

        ResourceManager::GetShader("depth_quad").Use().SetFloat("nearPlane", nearDistance);
        ResourceManager::GetShader("depth_quad").Use().SetFloat("farPlane", farDistance);
    }
};