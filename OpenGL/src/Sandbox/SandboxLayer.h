#pragma once

#include "Core/Layer.h"
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

    FrameBuffer multisampleFBO;
    FrameBuffer intermediateFBO;

private:
    const float m_AspectRatio = 4.0f / 3.0f;
    int m_ImageWidth = 800;
    int m_ImageHeight = static_cast<int>(m_ImageWidth / m_AspectRatio);
    // GLuint m_Texture;

    GLFWwindow* m_Window;
    GLuint m_Width;
    GLuint m_Height;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    Camera m_Camera;

    VertexBuffer planeVBO;
    VertexBuffer screenQuadVBO;

    void renderPlane(Shader shader);
    void renderModels(Shader shader);
    void renderQuad(Shader shader);
};