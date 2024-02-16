#include "Lighting/PointLight.h"

PointLight::PointLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
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
    ResourceManager::GetShader("debug_depth_cube_map").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetFloat("far_plane", farPlane);
}

void PointLight::RenderMesh()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::scale(model, glm::vec3(0.1f));
    ResourceManager::GetShader("lightSource").Use().SetMatrix4(1, model);
    ResourceManager::GetShader("lightSource").Use().SetVector3f("color", m_Color);
    m_Renderer->RenderCube(GL_TRIANGLES);
}

void PointLight::RenderGUI(int i)
{
    if (ImGui::TreeNodeEx(("Point Light " + std::to_string(i)).c_str())) {
        if (ImGui::DragFloat3("Position", (float*)&m_Position, 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_Position);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_Position);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_Position);
        }
        if (ImGui::ColorEdit3("Color", (float*)&m_Color)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        if (ImGui::DragFloat("Intensity", &m_Intensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        ImGui::Checkbox("Render Mesh", &m_RenderMesh);
        if (ImGui::Checkbox("Cast Shadows", &m_CastShadows)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
        }

        ImGui::TreePop();
    }
}