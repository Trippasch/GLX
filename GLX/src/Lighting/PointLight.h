#pragma once

#include "Resources/ResourceManager.h"
#include "Resources/FrameBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PointLight
{
public:
    glm::vec3 m_Position = glm::vec3(0.0f, 4.0f, 4.0f);
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 20.0f;

    bool m_CastShadows = true;
    GLuint m_ShadowWidth = 1024;
    GLuint m_ShadowHeight = 1024;
    glm::mat4 m_PointLightProjection;

    FrameBuffer m_DepthCubeMapFBO;

    PointLight()
    {
        ResourceManager::LoadShader("assets/shaders/depthCubeMapVS.glsl", "assets/shaders/depthCubeMapFS.glsl", "assets/shaders/depthCubeMapGS.glsl", "depth_cube_map");

        m_DepthCubeMapFBO.Bind();
        m_DepthCubeMapFBO.TextureAttachment(1, 2, GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();
    }

    ~PointLight()
    {
        m_DepthCubeMapFBO.Destroy();
    }

    void PointLightProjectionMatrix()
    {
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float aspect_ratio = static_cast<float>(m_ShadowWidth) / static_cast<float>(m_ShadowHeight);
        m_PointLightProjection = glm::perspective(glm::radians(90.0f), aspect_ratio, nearPlane, farPlane);

        ResourceManager::GetShader("depth_cube_map").Use().SetFloat("far_plane", farPlane);
        ResourceManager::GetShader("pbr_lighting").Use().SetFloat("far_plane", farPlane);
    }
};