#include "Sandbox/SandboxLayer.h"

#include "Core/Application.h"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    m_Camera = Camera(app.GetWindow().GetWidth(), app.GetWindow().GetHeight(), glm::vec3(0.0f, 0.0f, 5.0f));
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

    ResourceManager::LoadModel("assets/objects/backpack/backpack.obj", "backpack");
    ResourceManager::LoadTexture("assets/textures/wood.png", false, "wood");
    ResourceManager::LoadShader("assets/shaders/modelVS.glsl", "assets/shaders/modelFS.glsl", nullptr, "model");

    ResourceManager::GetShader("model").SetInteger("texture_diffuse1", 0);

    planeVBO = VertexBuffer(&planeVertices, sizeof(planeVertices), GL_STATIC_DRAW);

     //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SandboxLayer::OnUpdate()
{
    m_Camera.Inputs(m_Window);

    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("model").Use().SetMatrix4("projView", projView);

    // draw plane
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("wood").Bind();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ResourceManager::GetShader("model").Use().SetMatrix4("model", model);
    planeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    planeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    planeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVBO.UnlinkAttrib(0);
    planeVBO.UnlinkAttrib(1);
    planeVBO.UnlinkAttrib(2);
    glBindTexture(GL_TEXTURE_2D, 0);

    renderModels(ResourceManager::GetShader("model"));
}

void SandboxLayer::renderModels(Shader shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.Use().SetMatrix4("model", model);
    ResourceManager::GetModel("backpack").Draw(shader);
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
    ResourceManager::Clear();
    planeVBO.Destroy();
}