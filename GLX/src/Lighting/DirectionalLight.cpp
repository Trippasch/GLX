#include "Lighting/DirectionalLight.h"

DirectionalLight::DirectionalLight(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    m_ShadowCascadeLevels = std::vector<float>({
        m_Renderer->GetCamera().GetFarPlane() / 50.0f,
        m_Renderer->GetCamera().GetFarPlane() / 25.0f,
        m_Renderer->GetCamera().GetFarPlane() / 10.0f,
        m_Renderer->GetCamera().GetFarPlane() / 2.0f});

    m_DepthMapFBO.Bind();
    m_DepthMapFBO.DepthMapAttachment(1, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT32F, m_ShadowCascadeLevels.size() + 1, m_DepthMapResolution, m_DepthMapResolution);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    ResourceManager::GetShader("debug_depth_map").Use().SetFloat("nearPlane", m_Renderer->GetCamera().GetNearPlane());
    ResourceManager::GetShader("debug_depth_map").Use().SetFloat("farPlane", m_Renderer->GetCamera().GetFarPlane());
    ResourceManager::GetShader("debug_depth_map").Use().SetInteger("layer", m_DebugDepthMapLayer);
}

DirectionalLight::~DirectionalLight()
{
    m_DepthMapFBO.Destroy();
}

void DirectionalLight::InitLightUBO(UniformBuffer& lightsUBO)
{
    int offset = sizeof(glm::mat4x4) * 16 * MAX_LIGHTS;
    glm::vec4 shadowCascadeLevels;
    for (size_t i = 0; i < m_ShadowCascadeLevels.size(); i++) {
        shadowCascadeLevels[i] = m_ShadowCascadeLevels[i];
    }
    lightsUBO.FillBuffer(&shadowCascadeLevels, offset, sizeof(glm::vec4));

    int lightsOffset = sizeof(glm::mat4x4) * 16 * MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * MAX_LIGHTS + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS;
    size_t cascadeCount = m_ShadowCascadeLevels.size();
    lightsUBO.FillBuffer(&cascadeCount, lightsOffset + 2*sizeof(float), sizeof(float));
}

void DirectionalLight::RenderGUI(int i, UniformBuffer& lightsUBO)
{
    if (ImGui::TreeNodeEx(("Directional Light " + std::to_string(i)).c_str())) {
        int dirLightOffset = sizeof(glm::mat4x4) * 16 * MAX_LIGHTS + sizeof(glm::vec4) + i * 2 * sizeof(glm::vec4);
        if (ImGui::SliderFloat3("Direction", (float*)&m_Direction, -1.0f, 1.0f, "%.2f")) {
            lightsUBO.FillBuffer(&m_Direction, dirLightOffset, sizeof(glm::vec4));
        }
        if (ImGui::ColorEdit3("Color", (float*)&m_Color)) {
            glm::vec3 lightColor = m_Color * m_Intensity;
            lightsUBO.FillBuffer(&lightColor, dirLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        }
        if (ImGui::DragFloat("Intensity", &m_Intensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            glm::vec3 lightColor = m_Color * m_Intensity;
            lightsUBO.FillBuffer(&lightColor, dirLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        }
        if (ImGui::Checkbox("Cast Shadows", &m_CastShadows)) {
            int castShadows = static_cast<int>(m_CastShadows);
            lightsUBO.FillBuffer(&castShadows, dirLightOffset + sizeof(glm::vec4) + sizeof(glm::vec3), sizeof(float));
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
    Texture2D::UnBind(GL_TEXTURE_2D);
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
    const glm::mat4 proj = glm::perspective(glm::radians(m_Renderer->GetCamera().GetFov()), static_cast<float>(m_Renderer->GetWidth()) / m_Renderer->GetHeight(), nearPlane, far_plane);
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

    constexpr float zMult = 50.0f;
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

    const glm::mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProj * lightView;
}

std::vector<glm::mat4> DirectionalLight::GetLightSpaceMatrices()
{
    std::vector<glm::mat4> lightSpaceMatrices;
    for (size_t i = 0; i < m_ShadowCascadeLevels.size() + 1; i++) {
        if (i == 0) {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_Renderer->GetCamera().GetNearPlane(), m_ShadowCascadeLevels[i]));
        }
        else if (i < m_ShadowCascadeLevels.size()) {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_ShadowCascadeLevels[i - 1], m_ShadowCascadeLevels[i]));
        }
        else {
            lightSpaceMatrices.push_back(getLightSpaceMatrix(m_ShadowCascadeLevels[i - 1], m_Renderer->GetCamera().GetFarPlane()));
        }
    }

    return lightSpaceMatrices;
}