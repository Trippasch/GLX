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

    glm::vec3 m_Position = glm::vec3(0.0f, 4.0f, 4.0f);
    glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_Intensity = 20.0f;
    bool m_CastShadows = true;
    bool m_RenderMesh = true;
    GLuint m_DepthCubeMapResolution = 1024;

    glm::mat4 m_PointLightProjection;

    FrameBuffer m_DepthCubeMapFBO;

public:
    static constexpr int MAX_LIGHTS = 10;

    PointLight(RendererLayer* renderer);
    ~PointLight();

    void PointLightProjectionMatrix(float nearPlane, float farPlane);
    void RenderMesh();
    void RenderGUI(int i, UniformBuffer& lightsUBO);

    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetColor(const glm::vec3& color) { m_Color = color; }
    void SetIntensity(const float intensity) { m_Intensity = intensity; }
    void SetCastShadows(const bool castShadows) { m_CastShadows = castShadows; }
    void SetRenderMesh(const bool renderMesh) { m_RenderMesh = renderMesh; }
    void SetDepthCubeMapResolution(const GLuint resolution) { m_DepthCubeMapResolution = resolution; }
    void SetDepthCubeMapFBO(const FrameBuffer& depthCubeMapFBO) { m_DepthCubeMapFBO = depthCubeMapFBO; }
    void SetPointLightProjection(const glm::mat4& pointLightProjection) { m_PointLightProjection = pointLightProjection; }

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetColor() const { return m_Color; }
    const float& GetIntensity() const { return m_Intensity; }
    const bool& GetCastShadows() const { return m_CastShadows; }
    const bool& GetRenderMesh() const { return m_RenderMesh; }
    const GLuint& GetDepthCubeMapResolution() const { return m_DepthCubeMapResolution; }
    const FrameBuffer& GetDepthCubeMapFBO() const { return m_DepthCubeMapFBO; }
    const glm::mat4& GetPointLightProjection() const { return m_PointLightProjection; }
};