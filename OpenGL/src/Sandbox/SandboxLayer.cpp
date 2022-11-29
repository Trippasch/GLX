#include "Sandbox/SandboxLayer.h"

#include "Core/Application.h"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    m_Width = app.GetWindow().GetWidth();
    m_Height = app.GetWindow().GetHeight();
    m_Camera = Camera(m_Width, m_Height, glm::vec3(0.0f, 2.0f, 5.0f));
}

void SandboxLayer::OnAttach()
{
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    planeVBO = VertexBuffer(&planeVertices, sizeof(planeVertices), GL_STATIC_DRAW);

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    screenQuadVBO = VertexBuffer(&quadVertices, sizeof(quadVertices), GL_STATIC_DRAW);

    ResourceManager::LoadModel("assets/objects/backpack/backpack.obj", "backpack");
    ResourceManager::LoadTexture("assets/textures/old_floor.jpg", false, "floor");
    ResourceManager::LoadShader("assets/shaders/modelVS.glsl", "assets/shaders/modelFS.glsl", nullptr, "model");
    ResourceManager::LoadShader("assets/shaders/screenQuadVS.glsl", "assets/shaders/screenQuadFS.glsl", nullptr, "screen_quad");

    ResourceManager::GetShader("model").SetInteger("texture_diffuse1", 0);
    ResourceManager::GetShader("screen_quad").SetInteger("screenTexture", 0);

    multisampleFBO = FrameBuffer();
    multisampleFBO.Bind();
    multisampleFBO.TextureAttachment(1, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA, m_Width, m_Height);
    multisampleFBO.RenderBufferAttachment(GL_TRUE, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    intermediateFBO = FrameBuffer();
    intermediateFBO.Bind();
    intermediateFBO.TextureAttachment(1, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SandboxLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, static_cast<float>(m_Width) / m_Height, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("model").Use().SetMatrix4("projView", projView);

    glEnable(GL_DEPTH_TEST);

    multisampleFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderPlane(ResourceManager::GetShader("model"));
    renderModels(ResourceManager::GetShader("model"));

    multisampleFBO.Blit(intermediateFBO, m_Width, m_Height);
    multisampleFBO.UnBind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    intermediateFBO.BindTexture(0);
    renderQuad(ResourceManager::GetShader("screen_quad"));
    Texture2D::UnBind();
}

void SandboxLayer::renderPlane(Shader shader)
{
    ResourceManager::GetTexture("floor").Bind(0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.Use().SetMatrix4("model", model);

    planeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    planeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    planeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVBO.UnlinkAttrib(0);
    planeVBO.UnlinkAttrib(1);
    planeVBO.UnlinkAttrib(2);

    Texture2D::UnBind();
}

void SandboxLayer::renderModels(Shader shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.Use().SetMatrix4("model", model);
    ResourceManager::GetModel("backpack").Draw(shader);
}

void SandboxLayer::renderQuad(Shader shader)
{
    shader.Use();
    screenQuadVBO.LinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    screenQuadVBO.LinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    screenQuadVBO.UnlinkAttrib(0);
    screenQuadVBO.UnlinkAttrib(1);
}

void SandboxLayer::OnImGuiRender()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Generated Image");

    if (imGuiResize() == false) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    ImGui::Image((void*)(intptr_t)intermediateFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::PopStyleVar();

    if (ImGui::IsWindowFocused()) {
        m_Camera.Inputs((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle, deltaTime);
    }
    else {
        glfwSetInputMode((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_Camera.m_FirstClick = true;
    }

    ImGui::End();
}

bool SandboxLayer::imGuiResize()
{
    ImVec2 view = ImGui::GetContentRegionAvail();

    if (view.x != m_Width || view.y != m_Height) {
        if (view.x == 0 || view.y == 0) {
            return false;
        }

        m_Width = view.x;
        m_Height = view.y;

        GL_TRACE("Resizing window to {0}x{1}", m_Width, m_Height);
        glViewport(0, 0, m_Width, m_Height);

        multisampleFBO.Bind();
        multisampleFBO.ResizeTextureAttachment(GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA, m_Width, m_Height);
        multisampleFBO.ResizeRenderBufferAttachment(GL_TRUE, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        intermediateFBO.Bind();
        intermediateFBO.ResizeTextureAttachment(GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        return true;
    }
    return true;
}

void SandboxLayer::OnDetach()
{
    ResourceManager::Clear();

    planeVBO.Destroy();
    screenQuadVBO.Destroy();

    multisampleFBO.Destroy();
    intermediateFBO.Destroy();
}