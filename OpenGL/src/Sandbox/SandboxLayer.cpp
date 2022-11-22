#include "Sandbox/SandboxLayer.h"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
    // camera = Camera(m_ImageWidth, m_ImageHeight, glm::vec3(0.0f, 0.0f, 2.0f));
}

void SandboxLayer::OnAttach()
{
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    ResourceManager::LoadShader("assets/shaders/basicVS.glsl", "assets/shaders/basicFS.glsl", nullptr, "basic");

    cubeVAO = VertexArray();
    cubeVAO.Bind();

    cubeVBO = VertexBuffer(&vertices, sizeof(vertices), GL_STATIC_DRAW);
    cubeEBO = IndexBuffer(&indices, sizeof(indices), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    cubeVBO.UnBind();
    cubeVAO.UnBind();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SandboxLayer::OnUpdate()
{
    ResourceManager::GetShader("basic").Use();

    // glm::mat4 projView = camera.Matrix(45.0f, 0.1f, 100.0f);
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    // draw cubes
    // cubeEBO.Bind();
    // cubeVBO.LinkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    cubeVAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // cubeVBO.UnlinkAttrib(0);
    // cubeEBO.UnBind();
}

void SandboxLayer::OnImGuiRender()
{
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    // ImGui::Begin("Generated Image");

    // float viewport_width = ImGui::GetContentRegionAvail().x;
    // float viewport_height = ImGui::GetContentRegionAvail().y;

    // ImGui::Image((void*)(intptr_t)m_Texture, ImVec2(viewport_width, viewport_height), ImVec2(0, 1), ImVec2(1, 0));
    // ImGui::PopStyleVar();

    // IsWindowFocused() has a minor bug -- it centers the mouse when losing focus
    // if (ImGui::IsWindowFocused()) {

    //     m_Camera->Inputs((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle);

    //     glm::vec3 rightV = glm::normalize(glm::cross(m_Camera->m_Orientation, m_Camera->m_Up));
    //     glm::vec3 upV = glm::normalize(glm::cross(m_Camera->m_Orientation, rightV));

    //     m_Inputs.origin_x = m_Camera->m_Position.x;
    //     m_Inputs.origin_y = m_Camera->m_Position.y;
    //     m_Inputs.origin_z = m_Camera->m_Position.z;

    //     m_Inputs.orientation_x = m_Camera->m_Orientation.x;
    //     m_Inputs.orientation_y = m_Camera->m_Orientation.y;
    //     m_Inputs.orientation_z = m_Camera->m_Orientation.z;

    //     m_Inputs.up_x = upV.x;
    //     m_Inputs.up_y = upV.y;
    //     m_Inputs.up_z = upV.z;
    // }
    // else {
    //     glfwSetInputMode((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //     m_Camera->m_FirstClick = true;
    // }

    // ImGui::End();
}

void SandboxLayer::OnDetach()
{
    cubeVBO.Destroy();
    cubeEBO.Destroy();
}