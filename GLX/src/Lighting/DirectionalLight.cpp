#include "Lighting/DirectionalLight.h"

DirectionalLight::DirectionalLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    m_ShadowCascadeLevels = std::vector<float>({
        m_Renderer->GetCamera().m_FarPlane / 50.0f,
        m_Renderer->GetCamera().m_FarPlane / 25.0f,
        m_Renderer->GetCamera().m_FarPlane / 10.0f,
        m_Renderer->GetCamera().m_FarPlane / 2.0f});

    m_DepthMapFBO.Bind();
    m_DepthMapFBO.DepthMapAttachment(1, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT32F, m_ShadowCascadeLevels.size() + 1, m_DepthMapResolution, m_DepthMapResolution);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    ResourceManager::GetShader("debug_depth_map").Use().SetFloat("nearPlane", m_Renderer->GetCamera().m_NearPlane);
    ResourceManager::GetShader("debug_depth_map").Use().SetFloat("farPlane", m_Renderer->GetCamera().m_FarPlane);
    ResourceManager::GetShader("debug_depth_map").Use().SetInteger("layer", m_DebugDepthMapLayer);
}

DirectionalLight::~DirectionalLight()
{
    m_DepthMapFBO.Destroy();
}

void DirectionalLight::RenderGUI(int i)
{
    if (ImGui::TreeNodeEx(("Directional Light " + std::to_string(i)).c_str())) {
        if (ImGui::SliderFloat3("Direction", (float*)&m_Direction, -1.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].direction").c_str(), m_Direction);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].direction").c_str(), m_Direction);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].direction").c_str(), m_Direction);
        }
        if (ImGui::ColorEdit3("Color", (float*)&m_Color)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        if (ImGui::DragFloat("Intensity", &m_Intensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetVector3f(("dirLights[" + std::to_string(i) + "].color").c_str(), m_Color * m_Intensity);
        }
        if (ImGui::Checkbox("Cast Shadows", &m_CastShadows)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("dirLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger(("dirLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
            ResourceManager::GetShader("pbr_lighting_textured_gltf").Use().SetInteger(("dirLights[" + std::to_string(i) + "].shadows").c_str(), m_CastShadows);
        }
        if (ImGui::Button("Remove Light", ImVec2(0, 0))) {
            m_Renderer->RemoveLight(this);
        }

        ImGui::TreePop();
    }
}

void DirectionalLight::RenderDebugGUI()
{
    if (ImGui::SliderInt("Layer", &m_DebugDepthMapLayer, 0, m_ShadowCascadeLevels.size(), "%d")) {
        ResourceManager::GetShader("debug_depth_map").Use().SetInteger("layer", m_DebugDepthMapLayer);
    }
}

void DirectionalLight::RenderDepthMapQuad()
{
    glActiveTexture(GL_TEXTURE0);
    m_DepthMapFBO.BindTexture(GL_TEXTURE_2D_ARRAY, 0);
    ResourceManager::GetShader("debug_depth_map").Use();
    m_Renderer->RenderQuad(GL_TRIANGLES);
    Texture2D::UnBind();
}

std::vector<glm::vec4> DirectionalLight::getFrustumCornersWorldSpace(const glm::mat4& projView)
{
    const glm::mat4 invVP = glm::inverse(projView);
    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; x++) {
        for (unsigned int y = 0; y < 2; y++) {
            for (unsigned int z = 0; z < 2; z++) {
                const glm::vec4 corner = invVP * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(corner / corner.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<glm::vec4> DirectionalLight::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 DirectionalLight::getLightSpaceMatrix(const float nearPlane, const float far_plane)
{
    const glm::mat4 proj = glm::perspective(glm::radians(m_Renderer->GetCamera().m_Fov), static_cast<float>(m_Renderer->GetWidth()) / m_Renderer->GetHeight(), nearPlane, far_plane);
    const std::vector<glm::vec4> frustumCorners = getFrustumCornersWorldSpace(proj, m_Renderer->GetCamera().GetViewMatrix());

    glm::vec3 center = glm::vec3(0.0f);
    for (const auto& v : frustumCorners) {
        center += glm::vec3(v);
    }
    center /= frustumCorners.size();

    const glm::mat4 lightView = glm::lookAt(center + m_Direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : frustumCorners) {
        const auto vLightSpace = lightView * v;
        minX = std::min(minX, vLightSpace.x);
        maxX = std::max(maxX, vLightSpace.x);
        minY = std::min(minY, vLightSpace.y);
        maxY = std::max(maxY, vLightSpace.y);
        minZ = std::min(minZ, vLightSpace.z);
        maxZ = std::max(maxZ, vLightSpace.z);
    }

    constexpr float zMult = 100.0f;
    if (minZ < 0.0f) {
        minZ *= zMult;
    }
    else {
        minZ /= zMult;
    }
    if (maxZ < 0.0f) {
        maxZ /= zMult;
    }
    else {
        maxZ *= zMult;
    }

    const glm::mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, -1 * maxZ, -1 * minZ);
    return lightProj * lightView;
}

std::vector<glm::mat4> DirectionalLight::GetLightSpaceMatrices()
{
    std::vector<glm::mat4> lightSpaceMatrices;
    for (size_t i = 0; i < m_ShadowCascadeLevels.size() + 1; i++) {
        if (i == 0) {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_Renderer->GetCamera().m_NearPlane, m_ShadowCascadeLevels[i]));
        }
        else if (i < m_ShadowCascadeLevels.size()) {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_ShadowCascadeLevels[i - 1], m_ShadowCascadeLevels[i]));
        }
        else {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_ShadowCascadeLevels[i - 1], m_Renderer->GetCamera().m_FarPlane));
        }
    }

    return lightSpaceMatrices;
}