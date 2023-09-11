#pragma once

#include "Core/Layer.h"
#include "Core/Log.h"
#include "Core/ResourceManager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/Camera.h"

class SandboxLayer : public Layer
{
public:
    SandboxLayer();
    ~SandboxLayer() = default;

    virtual void OnAttach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;
    virtual void OnDetach() override;

    inline GLuint GetWidth() const { return m_Width; }
    inline void SetWidth(GLuint width) { this->m_Width = width; }
    inline GLuint GetHeight() const { return m_Height; }
    inline void SetHeight(GLuint height) { this->m_Height = height; }

private:
    GLFWwindow* m_Window;
    GLuint m_Width = 800;
    GLuint m_Height = 600;

    bool m_VSync = false;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    Camera m_Camera;

    VertexBuffer planeVBO;
    VertexBuffer cubeVBO;
    VertexBuffer sphereVBO;
    VertexBuffer screenQuadVBO;

    IndexBuffer sphereEBO;
    GLuint indexCount;

    FrameBuffer multisampleFBO;
    FrameBuffer hdrFBO;
    FrameBuffer pingpongFBO[2];
    FrameBuffer imguiFBO;
    FrameBuffer captureFBO;
    FrameBuffer depthMapFBO;
    FrameBuffer depthCubeMapFBO;
    FrameBuffer bloomFBO;

    Texture2D m_EnvCubemap;
    GLuint m_EnvCubemapWidth = 2048;
    GLuint m_EnvCubemapHeight = 2048;

    Texture2D m_Irradiancemap;
    GLuint m_IrradiancemapWidth = 32;
    GLuint m_IrradiancemapHeight = 32;

    Texture2D m_Prefiltermap;
    GLuint m_PrefiltermapWidth = 512;
    GLuint m_PrefiltermapHeight = 512;

    Texture2D m_BRDFLUTTexture;
    GLuint m_BRDFLUTTextureWidth = 512;
    GLuint m_BRDFLUTTextureHeight = 512;

    // Light Properties
    bool m_DebugDepthMap = false;
    bool m_DebugDepthCubeMap = false;

    // Directional light
    bool m_UseDirLight = true;
    bool m_UseDirShadows = true;
    glm::vec3 m_DirLightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 m_DirLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_DirLightIntensity = 1.0f;

    // Point lights
    bool m_UsePointLights = true;
    bool m_UsePointShadows = true;
    std::vector<glm::vec3> m_PointLightPositions;
    std::vector<glm::vec3> m_PointLightColors;
    std::vector<float> m_PointLightIntensities;

    // Shadows
    GLuint m_ShadowWidth = 4096;
    GLuint m_ShadowHeight = 4096;
    glm::vec3 m_DepthMapOrig = glm::vec3(0.0f);
    glm::mat4 m_DepthMapProjection;
    glm::mat4 m_PointLightProjection;

    // Post Processing
    bool m_UseGreyscale = false;
    bool m_UseInversion = false;
    bool m_UseRidge = false;
    bool m_UseEdge = false;
    bool m_UseSharpen = false;
    bool m_UseBlur = false;
    bool m_UseBloom = true;
    float m_Exposure = 0.5f;
    float m_BloomFilterRadius = 0.005f;
    float m_BloomStrength = 0.04f;
    bool m_KarisAverageOnDownsample = true;

    glm::vec2 m_SrcResolution = glm::vec2((float)m_Width, (float)m_Height);


    // PBR Material Properties
    glm::vec3 m_Albedo = glm::vec3(0.21f, 0.17f, 0.17f);
    float m_Metallic = 0.39f;
    float m_Roughness = 0.23f;
    float m_AO = 1.0f;

    // Object Properties
    glm::vec3 m_ObjectPosition = glm::vec3(0.0f, 2.0f, 4.0f);
    float m_ObjectScale = 1.0f;
    float m_RotationAngleX = 90.0f;
    float m_RotationAngleY = 0.0f;
    float m_RotationAngleZ = 0.0f;
    glm::vec3 m_RotationAngles = glm::vec3(m_RotationAngleX, m_RotationAngleY, m_RotationAngleZ);
    float m_EmissiveIntensity = 1.0f;

    // Skybox Properties
    std::string m_SkyboxFilename = "assets/textures/hdr-skyboxes/Nature_8K_hdri.jpg";

    void renderPlane();
    void renderCube();
    void renderSphere();
    void renderQuad();
    void renderObject(Shader shader, glm::mat4 model);

    void renderBloomTexture();

    bool imGuiResize();

    void createSkybox();
};