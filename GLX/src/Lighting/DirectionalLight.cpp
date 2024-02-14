#include "Lighting/DirectionalLight.h"

DirectionalLight::DirectionalLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    ResourceManager::LoadShader("assets/shaders/depthMapVS.glsl", "assets/shaders/depthMapFS.glsl", nullptr, "depth_map");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/depthQuadFS.glsl", nullptr, "depth_quad");

    m_DepthMapFBO.Bind();
    m_DepthMapFBO.TextureAttachment(1, 1, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

DirectionalLight::~DirectionalLight()
{
    m_DepthMapFBO.Destroy();
}

void DirectionalLight::DirectionalLightProjectionMatrix()
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

void DirectionalLight::RenderGUI(int i)
{
    if (ImGui::TreeNodeEx(("Directional Light " + std::to_string(i)).c_str())) {
        if (ImGui::SliderFloat3("Direction", (float*)&m_Direction, -1.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].direction").c_str(), m_Direction);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].direction").c_str(), m_Direction);
        }
        if (ImGui::ColorEdit3("Color", (float*)&m_Color)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        if (ImGui::DragFloat("Intensity", &m_Intensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        if (ImGui::Checkbox("Cast Shadows", &m_CastShadows)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("dirLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger(("dirLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
        }

        ImGui::TreePop();
    }
}

void DirectionalLight::RenderDepthMapQuad()
{
    glActiveTexture(GL_TEXTURE0);
    m_DepthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    ResourceManager::GetShader("depth_quad").Use();
    m_Renderer->RenderQuad(GL_TRIANGLES);
    Texture2D::UnBind();
}