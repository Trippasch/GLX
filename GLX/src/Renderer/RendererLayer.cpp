#include "Renderer/RendererLayer.h"

#include "Core/Application.h"

#include "Utils/glfw_tools.h"
#include "Utils/gl_tools.h"

RendererLayer::RendererLayer()
    : Layer("RendererLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    m_Camera = Camera(m_Width, m_Height, glm::vec3(0.0f, 2.0f, 20.0f));
}

void RendererLayer::OnAttach()
{
    // Set important OpenGL states
    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // Set default viewport
    glViewport(0, 0, m_Width, m_Height);

    ResourceManager::LoadModel("assets/objects/helmet/DamagedHelmet.gltf", "helmet");
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "lightSource");
    ResourceManager::GetShader("lightSource").Use().SetVector3f("color", glm::vec3(1.0f, 1.0f, 1.0f));
    ResourceManager::LoadShader("assets/shaders/gizmoVS.glsl", "assets/shaders/gizmoFS.glsl", nullptr, "gizmo");
    ResourceManager::GetShader("gizmo").Use().SetVector3f("color", glm::vec3(1.0f, 1.0f, 1.0f));

    // Initialize Framebuffers
    m_MultisampleFBO.Bind();
    m_MultisampleFBO.TextureAttachment(2, 0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA16F, m_Width, m_Height);
    m_MultisampleFBO.RenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    m_ImGUIFBO.Bind();
    m_ImGUIFBO.TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    // Renderer Library
    m_RendererLibrary = new RendererLibrary();

    // PBR
    m_PBR = new PBR(this);
    if (m_UseSkybox) {
        m_PBR->CreateSkybox();
    }

    // PostProcessor
    m_PostProcessor = new PostProcessor(this);

    // Lights
    DirectionalLight* dirLight = new DirectionalLight();
    AddLight(dirLight);
    PointLight* pointLight = new PointLight();
    AddLight(pointLight);

    Entity* lastEntity;
    // Create Models
    m_Models.addChild<ModelEntity>(ResourceManager::GetModel("helmet"));
    lastEntity = m_Models.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 2.0f, 4.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec3(1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(1.0f);

    // Create Planes
    m_Planes.addChild<Plane>(m_RendererLibrary->GetPlaneVBO(), m_RendererLibrary->GetPlaneVertices(), 48);
    lastEntity = m_Planes.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(10.0f));
    lastEntity->material.setAlbedo(glm::vec3(0.21f, 0.17f, 0.17f));
    lastEntity->material.setMetallic(0.39f);
    lastEntity->material.setRoughness(0.23f);
    lastEntity->material.setAO(1.0f);

    // Create Cubes
    m_Cubes.addChild<Cube>(m_RendererLibrary->GetCubeVBO(), m_RendererLibrary->GetCubeVertices(), 288);
    lastEntity = m_Cubes.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(4.0f, 2.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec3(0.21f, 0.17f, 0.17f));
    lastEntity->material.setMetallic(0.39f);
    lastEntity->material.setRoughness(0.23f);
    lastEntity->material.setAO(1.0f);

    // Create Spheres
    m_Spheres.addChild<Sphere>(m_RendererLibrary->GetSphereVBO(), &m_RendererLibrary->GetSphereVertices()[0], m_RendererLibrary->GetSphereVertices().size(), m_RendererLibrary->GetSphereEBO(), m_RendererLibrary->GetSphereIndicesCount());
    lastEntity = m_Spheres.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(-4.0f, 2.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec3(0.21f, 0.17f, 0.17f));
    lastEntity->material.setMetallic(0.39f);
    lastEntity->material.setRoughness(0.23f);
    lastEntity->material.setAO(1.0f);
}

void RendererLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, static_cast<float>(m_Width) / m_Height, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    m_CamFrustum = m_Camera.CreateFrustumFromCamera(static_cast<float>(m_Width) / m_Height, glm::radians(m_Camera.m_Fov), m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("lightSource").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("gizmo").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("camPos", m_Camera.m_Position);

    unsigned int total = 0, display = 0;

    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        // generate the depth map for directional shadows
        glm::mat4 depthMapView = glm::lookAt(m_DirectionalLights[i]->m_DepthMapOrig, m_DirectionalLights[i]->m_DepthMapOrig + m_DirectionalLights[i]->m_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 depthMapLightSpaceMatrix = m_DirectionalLights[i]->m_DepthMapProjection * depthMapView;

        ResourceManager::GetShader("depth_map").Use().SetMatrix4(0, depthMapLightSpaceMatrix);
        ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(2, depthMapLightSpaceMatrix);

        // shadow mapping
        glViewport(0, 0, m_DirectionalLights[i]->m_ShadowWidth, m_DirectionalLights[i]->m_ShadowHeight);

        // directional shadows
        m_DirectionalLights[i]->m_DepthMapFBO.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_Planes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_map"), m_CamFrustum, display, total);

        m_Spheres.renderSceneGraph(GL_TRIANGLE_STRIP, ResourceManager::GetShader("depth_map"), m_CamFrustum, display, total);

        m_Cubes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_map"), m_CamFrustum, display, total);

        m_Models.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_map"), m_CamFrustum, display, total);

        FrameBuffer::UnBind();

        glViewport(0, 0, m_Width, m_Height);
    }

    for (size_t i = 0; i < m_PointLights.size(); i++) {
        // generate depth cube map
        std::vector<glm::mat4> pointLightMatrices;
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLights[i]->m_PointLightProjection * glm::lookAt(m_PointLights[i]->m_Position, m_PointLights[i]->m_Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        for (GLuint j = 0; j < 6; j++) {
            ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), pointLightMatrices[j]);
        }

        ResourceManager::GetShader("depth_cube_map").Use().SetVector3f("lightPos", m_PointLights[i]->m_Position);

        // shadow mapping
        glViewport(0, 0, m_PointLights[i]->m_ShadowWidth, m_PointLights[i]->m_ShadowHeight);

        // point shadows
        m_PointLights[i]->m_DepthCubeMapFBO.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_Planes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"), m_CamFrustum, display, total);

        m_Spheres.renderSceneGraph(GL_TRIANGLE_STRIP, ResourceManager::GetShader("depth_cube_map"), m_CamFrustum, display, total);

        m_Cubes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"), m_CamFrustum, display, total);

        m_Models.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"), m_CamFrustum, display, total);

        FrameBuffer::UnBind();

        glViewport(0, 0, m_Width, m_Height);
    }

    // Render Normal Scene
    m_MultisampleFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(4.0f, 2.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(1.0f));
    // ResourceManager::GetShader("lightSource").Use().SetMatrix4(1, model);
    // RenderCube(GL_TRIANGLES);

    if (m_UseSkybox) {
        m_PBR->GetIrradiancemap().BindCubemap(0);
        m_PBR->GetPrefiltermap().BindCubemap(1);
        m_PBR->GetBRDFLUTTexture().Bind(2);
    }
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        glActiveTexture(GL_TEXTURE9);
        m_DirectionalLights[i]->m_DepthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        glActiveTexture(GL_TEXTURE10);
        m_PointLights[i]->m_DepthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    m_Planes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("pbr_lighting"), m_CamFrustum, display, total);
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    if (m_UseSkybox) {
        m_PBR->GetIrradiancemap().BindCubemap(0);
        m_PBR->GetPrefiltermap().BindCubemap(1);
        m_PBR->GetBRDFLUTTexture().Bind(2);
    }
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        glActiveTexture(GL_TEXTURE9);
        m_DirectionalLights[i]->m_DepthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        glActiveTexture(GL_TEXTURE10);
        m_PointLights[i]->m_DepthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    m_Cubes.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("pbr_lighting"), m_CamFrustum, display, total);
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    if (m_UseSkybox) {
        m_PBR->GetIrradiancemap().BindCubemap(0);
        m_PBR->GetPrefiltermap().BindCubemap(1);
        m_PBR->GetBRDFLUTTexture().Bind(2);
    }
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        glActiveTexture(GL_TEXTURE9);
        m_DirectionalLights[i]->m_DepthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        glActiveTexture(GL_TEXTURE10);
        m_PointLights[i]->m_DepthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    m_Spheres.renderSceneGraph(GL_TRIANGLE_STRIP, ResourceManager::GetShader("pbr_lighting"), m_CamFrustum, display, total);
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.isGLTF", 1);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.isTextured", 1);
    if (m_UseSkybox) {
        m_PBR->GetIrradiancemap().BindCubemap(0);
        m_PBR->GetPrefiltermap().BindCubemap(1);
        m_PBR->GetBRDFLUTTexture().Bind(2);
    }
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        glActiveTexture(GL_TEXTURE9);
        m_DirectionalLights[i]->m_DepthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        glActiveTexture(GL_TEXTURE10);
        m_PointLights[i]->m_DepthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    m_Models.renderSceneGraph(GL_TRIANGLES, ResourceManager::GetShader("pbr_lighting"), m_CamFrustum, display, total);
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.isGLTF", 0);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.isTextured", 0);

    GL_TRACE("Total process in CPU : {0} / Total send to GPU : {1}", total, display);

    if (m_UseSkybox) {
        m_PBR->RenderSkybox();
    }
    FrameBuffer::UnBind();

    m_MultisampleFBO.Blit(m_PostProcessor->GetHDRFBO(), m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    if (m_PostProcessor->GetUseBloom()) {
        m_PostProcessor->RenderBloomTexture();
    }

    // Bind to ImGui Image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    m_ImGUIFBO.Bind();
    m_PostProcessor->RenderPostProcessingQuad();
    FrameBuffer::UnBind();
    glEnable(GL_DEPTH_TEST);
}

void RendererLayer::OnImGuiRender()
{
    Application& app = Application::Get();

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

    ImGui::ImageButton((void*)(intptr_t)m_ImGUIFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0), 0);
    ImGui::PopStyleVar();

    if (ImGui::IsWindowHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.MouseWheel) {
            m_Camera.ProcessMouseScroll(io.MouseWheel);
        }
    }

    if (ImGui::IsWindowFocused()) {
        m_Camera.Inputs((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle, deltaTime);
    }
    else {
        glfwSetInputMode((GLFWwindow *)ImGui::GetMainViewport()->PlatformHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_Camera.m_FirstClick = true;
    }

    ImGui::End();

    app.m_Console.Draw("Console");

    ImGui::Begin("Profiling");
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Generated image dimensions: %dx%d", m_Width, m_Height);
    ImGui::Separator();
    if (ImGui::Checkbox("Enable V-Sync", &m_VSync)) {
        if (m_VSync) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }
    }
    ImGui::Separator();
    ImGui::Text("Application average\n %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Separator();
#ifdef GL_DEBUG
    ImGui::Text("Running on Debug mode");
#elif GL_RELEASE
    ImGui::Text("Running on Release mode");
#elif GL_DIST
    ImGui::Text("Running on Dist mode");
#endif

    ImGui::End();

    ImGui::Begin("Application Info");

    printGLInfo();
    ImGui::Separator();
    printGLFWInfo(m_Window);
    ImGui::Separator();
    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());

    ImGui::End();

    ImGui::Begin("Settings");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Camera", base_flags)) {
        ImGui::DragFloat3("Position", (float*)&m_Camera.m_Position, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::DragFloat3("Orientation", (float*)&m_Camera.m_Orientation, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::SliderFloat("Field of view", &m_Camera.m_Fov, 1.0f, 90.0f, "%.2f");
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("3D Objects", base_flags)) {
        for (unsigned int i = 0; i < m_Planes.children.size(); i++) {
            if (ImGui::TreeNodeEx(("Plane " + std::to_string(i)).c_str())) {
                if (ImGui::DragFloat3("Position", (float*)&m_Planes.children[i].get()->transform.getLocalPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                    m_Planes.children[i].get()->transform.setLocalPosition(m_Planes.children[i].get()->transform.getLocalPosition());
                }
                if (ImGui::DragFloat3("Rotation", (float*)&m_Planes.children[i].get()->transform.getLocalRotation(), 0.01f, -180.0f, 180.0f, "%.2f")) {
                    m_Planes.children[i].get()->transform.setLocalRotation(m_Planes.children[i].get()->transform.getLocalRotation());
                }
                if (ImGui::DragFloat3("Scale", (float*)&m_Planes.children[i].get()->transform.getLocalScale(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                    m_Planes.children[i].get()->transform.setLocalScale(m_Planes.children[i].get()->transform.getLocalScale());
                }
                if (ImGui::TreeNodeEx("Material")) {
                    if (ImGui::ColorEdit3("Albedo", (float*)&m_Planes.children[i].get()->material.getAlbedo())) {
                        m_Planes.children[i].get()->material.setAlbedo(m_Planes.children[i].get()->material.getAlbedo());
                    }
                    if (ImGui::SliderFloat("Metallic", (float*)&m_Planes.children[i].get()->material.getMetallic(), 0.0f, 1.0f, "%.2f")) {
                        m_Planes.children[i].get()->material.setMetallic(m_Planes.children[i].get()->material.getMetallic());
                    }
                    if (ImGui::SliderFloat("Roughness", (float*)&m_Planes.children[i].get()->material.getRoughness(), 0.0f, 1.0f, "%.2f")) {
                        m_Planes.children[i].get()->material.setRoughness(m_Planes.children[i].get()->material.getRoughness());
                    }
                    if (ImGui::SliderFloat("AO", (float*)&m_Planes.children[i].get()->material.getAO(), 0.0f, 1.0f, "%.2f")) {
                        m_Planes.children[i].get()->material.setAO(m_Planes.children[i].get()->material.getAO());
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        for (unsigned int i = 0; i < m_Models.children.size(); i++) {
            if (ImGui::TreeNodeEx(("Model " + std::to_string(i)).c_str())) {
                if (ImGui::DragFloat3("Position", (float*)&m_Models.children[i].get()->transform.getLocalPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                    m_Models.children[i].get()->transform.setLocalPosition(m_Models.children[i].get()->transform.getLocalPosition());
                }
                if (ImGui::DragFloat3("Rotation", (float*)&m_Models.children[i].get()->transform.getLocalRotation(), 0.01f, -180.0f, 180.0f, "%.2f")) {
                    m_Models.children[i].get()->transform.setLocalRotation(m_Models.children[i].get()->transform.getLocalRotation());
                }
                if (ImGui::DragFloat3("Scale", (float*)&m_Models.children[i].get()->transform.getLocalScale(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                    m_Models.children[i].get()->transform.setLocalScale(m_Models.children[i].get()->transform.getLocalScale());
                }
                if (ImGui::TreeNodeEx("Material")) {
                    if (ImGui::ColorEdit3("Albedo", (float*)&m_Models.children[i].get()->material.getAlbedo())) {
                        m_Models.children[i].get()->material.setAlbedo(m_Models.children[i].get()->material.getAlbedo());
                    }
                    if (ImGui::SliderFloat("Metallic", (float*)&m_Models.children[i].get()->material.getMetallic(), 0.0f, 1.0f, "%.2f")) {
                        m_Models.children[i].get()->material.setMetallic(m_Models.children[i].get()->material.getMetallic());
                    }
                    if (ImGui::SliderFloat("Roughness", (float*)&m_Models.children[i].get()->material.getRoughness(), 0.0f, 1.0f, "%.2f")) {
                        m_Models.children[i].get()->material.setRoughness(m_Models.children[i].get()->material.getRoughness());
                    }
                    if (ImGui::SliderFloat("AO", (float*)&m_Models.children[i].get()->material.getAO(), 0.0f, 1.0f, "%.2f")) {
                        m_Models.children[i].get()->material.setAO(m_Models.children[i].get()->material.getAO());
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        for (unsigned int i = 0; i < m_Cubes.children.size(); i++) {
            if (ImGui::TreeNodeEx(("Cube " + std::to_string(i)).c_str())) {
                if (ImGui::DragFloat3("Position", (float*)&m_Cubes.children[i].get()->transform.getLocalPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                    m_Cubes.children[i].get()->transform.setLocalPosition(m_Cubes.children[i].get()->transform.getLocalPosition());
                }
                if (ImGui::DragFloat3("Rotation", (float*)&m_Cubes.children[i].get()->transform.getLocalRotation(), 0.01f, -180.0f, 180.0f, "%.2f")) {
                    m_Cubes.children[i].get()->transform.setLocalRotation(m_Cubes.children[i].get()->transform.getLocalRotation());
                }
                if (ImGui::DragFloat3("Scale", (float*)&m_Cubes.children[i].get()->transform.getLocalScale(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                    m_Cubes.children[i].get()->transform.setLocalScale(m_Cubes.children[i].get()->transform.getLocalScale());
                }
                if (ImGui::TreeNodeEx("Material")) {
                    if (ImGui::ColorEdit3("Albedo", (float*)&m_Cubes.children[i].get()->material.getAlbedo())) {
                        m_Cubes.children[i].get()->material.setAlbedo(m_Cubes.children[i].get()->material.getAlbedo());
                    }
                    if (ImGui::SliderFloat("Metallic", (float*)&m_Cubes.children[i].get()->material.getMetallic(), 0.0f, 1.0f, "%.2f")) {
                        m_Cubes.children[i].get()->material.setMetallic(m_Cubes.children[i].get()->material.getMetallic());
                    }
                    if (ImGui::SliderFloat("Roughness", (float*)&m_Cubes.children[i].get()->material.getRoughness(), 0.0f, 1.0f, "%.2f")) {
                        m_Cubes.children[i].get()->material.setRoughness(m_Cubes.children[i].get()->material.getRoughness());
                    }
                    if (ImGui::SliderFloat("AO", (float*)&m_Cubes.children[i].get()->material.getAO(), 0.0f, 1.0f, "%.2f")) {
                        m_Cubes.children[i].get()->material.setAO(m_Cubes.children[i].get()->material.getAO());
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        for (unsigned int i = 0; i < m_Spheres.children.size(); i++) {
            if (ImGui::TreeNodeEx(("Sphere " + std::to_string(i)).c_str())) {
                if (ImGui::DragFloat3("Position", (float*)&m_Spheres.children[i].get()->transform.getLocalPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                    m_Spheres.children[i].get()->transform.setLocalPosition(m_Spheres.children[i].get()->transform.getLocalPosition());
                }
                if (ImGui::DragFloat3("Rotation", (float*)&m_Spheres.children[i].get()->transform.getLocalRotation(), 0.01f, -180.0f, 180.0f, "%.2f")) {
                    m_Spheres.children[i].get()->transform.setLocalRotation(m_Spheres.children[i].get()->transform.getLocalRotation());
                }
                if (ImGui::DragFloat3("Scale", (float*)&m_Spheres.children[i].get()->transform.getLocalScale(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                    m_Spheres.children[i].get()->transform.setLocalScale(m_Spheres.children[i].get()->transform.getLocalScale());
                }
                if (ImGui::TreeNodeEx("Material")) {
                    if (ImGui::ColorEdit3("Albedo", (float*)&m_Spheres.children[i].get()->material.getAlbedo())) {
                        m_Spheres.children[i].get()->material.setAlbedo(m_Spheres.children[i].get()->material.getAlbedo());
                    }
                    if (ImGui::SliderFloat("Metallic", (float*)&m_Spheres.children[i].get()->material.getMetallic(), 0.0f, 1.0f, "%.2f")) {
                        m_Spheres.children[i].get()->material.setMetallic(m_Spheres.children[i].get()->material.getMetallic());
                    }
                    if (ImGui::SliderFloat("Roughness", (float*)&m_Spheres.children[i].get()->material.getRoughness(), 0.0f, 1.0f, "%.2f")) {
                        m_Spheres.children[i].get()->material.setRoughness(m_Spheres.children[i].get()->material.getRoughness());
                    }
                    if (ImGui::SliderFloat("AO", (float*)&m_Spheres.children[i].get()->material.getAO(), 0.0f, 1.0f, "%.2f")) {
                        m_Spheres.children[i].get()->material.setAO(m_Spheres.children[i].get()->material.getAO());
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Background", base_flags)) {
        if (ImGui::TreeNodeEx("Use Skybox", base_flags)) {
            ImGui::SameLine();
            ImGuiLayer::ToggleButton(" ", &m_UseSkybox);
            if (m_UseSkybox) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.useIBL", 1);
                m_PBR->RenderSkyboxGUI();
            }
            else {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.useIBL", 0);
            }
            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    ImGui::End();

    m_PostProcessor->RenderPostProcessingGUI();
}

bool RendererLayer::imGuiResize()
{
    ImVec2 view = ImGui::GetContentRegionAvail();

    if (view.x != m_Width || view.y != m_Height) {
        if (view.x == 0 || view.y == 0) {
            return false;
        }

        m_Width = view.x;
        m_Height = view.y;

        glViewport(0, 0, m_Width, m_Height);

        m_MultisampleFBO.Bind();
        m_MultisampleFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA16F, m_Width, m_Height);
        m_MultisampleFBO.ResizeRenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        m_ImGUIFBO.Bind();
        m_ImGUIFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        m_PostProcessor->ResizeBuffers(m_Width, m_Height);

        GL_INFO("Resizing Generated Image to : {0} {1}", m_Width, m_Height);

        return true;
    }
    return true;
}

void RendererLayer::OnDetach()
{
    ResourceManager::Clear();
    m_ImGUIFBO.Destroy();
    m_MultisampleFBO.Destroy();

    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        delete m_DirectionalLights[i];
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        delete m_PointLights[i];
    }

    delete m_PBR;
    delete m_RendererLibrary;
    delete m_PostProcessor;
}

void RendererLayer::RenderCube(GLenum mode)
{
    m_RendererLibrary->GetCubeVBO().LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    m_RendererLibrary->GetCubeVBO().LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    m_RendererLibrary->GetCubeVBO().LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(mode, 0, 36);
    m_RendererLibrary->GetCubeVBO().UnlinkAttrib(0);
    m_RendererLibrary->GetCubeVBO().UnlinkAttrib(1);
    m_RendererLibrary->GetCubeVBO().UnlinkAttrib(2);
}

void RendererLayer::RenderQuad(GLenum mode)
{
    m_RendererLibrary->GetQuadVBO().LinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    m_RendererLibrary->GetQuadVBO().LinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glDrawArrays(mode, 0, 6);
    m_RendererLibrary->GetQuadVBO().UnlinkAttrib(0);
    m_RendererLibrary->GetQuadVBO().UnlinkAttrib(1);
}

void RendererLayer::AddLight(DirectionalLight* light)
{
    m_DirectionalLights.push_back(light);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.use", 1);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.direction", light->m_Direction);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.color", light->m_Color * light->m_Intensity);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.shadows", light->m_CastShadows);
    light->DirectionalLightProjectionMatrix();
}

void RendererLayer::AddLight(PointLight* light)
{
    m_PointLights.push_back(light);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("pointLight.use", 1);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugDepthCubeMap", 0);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(m_PointLights.size()-1) + "].position").c_str(), light->m_Position);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(m_PointLights.size()-1) + "].color").c_str(), light->m_Color * light->m_Intensity);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("pointLights[" + std::to_string(m_PointLights.size()-1) + "].shadows").c_str(), light->m_CastShadows);
    light->PointLightProjectionMatrix();
}