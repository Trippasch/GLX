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
    FrameBuffer intermediateFBO;
    FrameBuffer imguiFBO;
    FrameBuffer captureFBO;

    Texture2D m_EnvCubemap;
    GLuint m_EnvCubemapWidth = 2048;
    GLuint m_EnvCubemapHeight = 2048;

    Texture2D m_Irradiancemap;
    GLuint m_IrradiancemapWidth = 32;
    GLuint m_IrradiancemapHeight = 32;

    Texture2D m_Prefiltermap;
    GLuint m_PrefiltermapWidth = 128;
    GLuint m_PrefiltermapHeight = 128;

    Texture2D m_BRDFLUTTexture;
    GLuint m_BRDFLUTTextureWidth = 512;
    GLuint m_BRDFLUTTextureHeight = 512;

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    bool m_UseGreyscale = false;
    bool m_UseInversion = false;
    bool m_UseRidge = false;
    bool m_UseEdge = false;
    bool m_UseSharpen = false;
    bool m_UseBlur = false;

    glm::vec3 m_Albedo = glm::vec3(0.5f, 0.0f, 0.0f);
    float m_Metallic = 0.0f;
    float m_Roughness = 0.0f;
    float m_AO = 1.0f;

    void renderPlane();
    void renderCube();
    void renderSphere();
    void renderQuad();
    void renderModel(Shader shader);

    bool imGuiResize();
};