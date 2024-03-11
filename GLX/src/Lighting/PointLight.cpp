#include "Lighting/PointLight.h"

PointLight::PointLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    m_DepthCubeMapFBO.Bind();
    m_DepthCubeMapFBO.TextureAttachment(1, 2, GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, m_DepthCubeMapResolution, m_DepthCubeMapResolution);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

PointLight::~PointLight()
{
    m_DepthCubeMapFBO.Destroy();
}

void PointLight::PointLightProjectionMatrix(float nearPlane, float farPlane)
{
    float aspect_ratio = static_cast<float>(m_DepthCubeMapResolution) / static_cast<float>(m_DepthCubeMapResolution);
    m_PointLightProjection = glm::perspective(glm::radians(90.0f), aspect_ratio, nearPlane, farPlane);
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

void PointLight::RenderGUI(int i, UniformBuffer& lightsUBO)
{
    if (ImGui::TreeNodeEx(("Point Light " + std::to_string(i)).c_str())) {
        int pointLightOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + i * 2 * sizeof(glm::vec4);
        if (ImGui::DragFloat3("Position", (float*)&m_Position, 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
            lightsUBO.FillBuffer(&m_Position, pointLightOffset, sizeof(glm::vec4));
        }
        if (ImGui::ColorEdit3("Color", (float*)&m_Color)) {
            glm::vec3 lightColor = m_Color * m_Intensity;
            lightsUBO.FillBuffer(&lightColor, pointLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        }
        if (ImGui::DragFloat("Intensity", &m_Intensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            glm::vec3 lightColor = m_Color * m_Intensity;
            lightsUBO.FillBuffer(&lightColor, pointLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        }
        ImGui::Checkbox("Render Mesh", &m_RenderMesh);
        if (ImGui::Checkbox("Cast Shadows", &m_CastShadows)) {
            int castShadows = static_cast<int>(m_CastShadows);
            lightsUBO.FillBuffer(&castShadows, pointLightOffset + sizeof(glm::vec4) + sizeof(glm::vec3), sizeof(float));
        }
        if (ImGui::Button("Remove Light", ImVec2(0, 0))) {
            m_Renderer->RemoveLight(this);
        }

        ImGui::TreePop();
    }
}