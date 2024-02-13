#include "Lighting/PointLight.h"

PointLight::PointLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    ResourceManager::LoadShader("assets/shaders/depthCubeMapVS.glsl", "assets/shaders/depthCubeMapFS.glsl", "assets/shaders/depthCubeMapGS.glsl", "depth_cube_map");
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "lightSource");
    ResourceManager::GetShader("lightSource").Use().SetVector3f("color", m_Color);
    ResourceManager::GetShader("lightSource").Use().SetBlockIndex("Matrices", 0);

    m_DepthCubeMapFBO.Bind();
    m_DepthCubeMapFBO.TextureAttachment(1, 2, GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

PointLight::~PointLight()
{
    m_DepthCubeMapFBO.Destroy();
}

void PointLight::PointLightProjectionMatrix()
{
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspect_ratio = static_cast<float>(m_ShadowWidth) / static_cast<float>(m_ShadowHeight);
    m_PointLightProjection = glm::perspective(glm::radians(90.0f), aspect_ratio, nearPlane, farPlane);

    ResourceManager::GetShader("depth_cube_map").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("far_plane", farPlane);
}

void PointLight::RenderMesh()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::scale(model, glm::vec3(0.3f));
    ResourceManager::GetShader("lightSource").Use().SetMatrix4(1, model);
    m_Renderer->RenderCube(GL_TRIANGLES);
}