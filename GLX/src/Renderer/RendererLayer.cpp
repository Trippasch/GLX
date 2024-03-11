#include "Renderer/RendererLayer.h"

#include "Core/Application.h"

#include "Utils/glfw_tools.h"
#include "Utils/gl_tools.h"

RendererLayer::RendererLayer()
    : Layer("RendererLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
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

    // Initialize Framebuffers
    m_MultisampleFBO.Bind();
    m_MultisampleFBO.TextureAttachment(2, 0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA16F, m_Width, m_Height);
    m_MultisampleFBO.TranslucentAttachments(GL_TEXTURE_2D_MULTISAMPLE, m_Width, m_Height);
    m_MultisampleFBO.RenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    m_ImGUIFBO.Bind();
    m_ImGUIFBO.TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    m_DebugFBO.Bind();
    m_DebugFBO.TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    // Uniform Buffers
    m_MatricesUBO = UniformBuffer(0, 0, sizeof(glm::mat4), GL_DYNAMIC_DRAW);
    m_LightsUBO = UniformBuffer(1, 0, sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4)
                                            + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS
                                            + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS + sizeof(glm::vec4), GL_DYNAMIC_DRAW);
    m_CameraUBO = UniformBuffer(2, 0, sizeof(glm::mat4) + sizeof(glm::vec4), GL_DYNAMIC_DRAW);
    m_ObjectUBO = UniformBuffer(3, 0, sizeof(glm::vec4) + Material::MAX_OBJECTS*(2*sizeof(glm::vec4)), GL_DYNAMIC_DRAW);

    // Fill Uniform Buffers
    m_ObjectUBO.FillBuffer(&m_UseSkybox, 0, sizeof(float));

    // Renderer Library
    m_RendererLibrary = new RendererLibrary();

    // PBR
    m_PBR = new PBR(this);
    if (m_UseSkybox) {
        m_PBR->CreateSkybox();
    }

    // PostProcessor
    m_PostProcessor = new PostProcessor(this);

    // Camera
    m_Camera = Camera(m_Width, m_Height, glm::vec3(8.0f, 2.5f, -2.5f));
    m_Camera.SetOrientation(glm::vec3(-1.0f, 0.0f, 0.0f));

    // Lights
    DirectionalLight* dirLight = new DirectionalLight(this);
    dirLight->SetDirection(glm::vec3(0.35f, 0.87f, 0.15f));
    dirLight->SetColor(glm::vec3(0.94f, 0.65f, 0.20f));
    dirLight->SetIntensity(10.0f);
    AddLight(dirLight);
    PointLight* pointLight = new PointLight(this);
    pointLight->SetPosition(glm::vec3(-31.0f, 7.5f, 6.1f));
    pointLight->SetColor(glm::vec3(1.0f, 0.7f, 0.0f));
    pointLight->SetIntensity(20.0f);
    AddLight(pointLight);

    // Objects
    Entity* lastEntity;
    // Create Models
    m_Models.addChild<ModelEntity>(ResourceManager::GetModel("sponza"));
    lastEntity = m_Models.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 0.0f, -1.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(0.05f));
    lastEntity->material.setAlbedo(glm::vec4(1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(0.20f);
    lastEntity->material.setEmissive(0.0f);
    lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
    m_ObjectsID++;
    lastEntity->material.setIsTextured(true);
    lastEntity->material.setIsTranslucent(false);
    lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
    lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
    lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
    lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
    lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
    lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
    lastEntity->isGLTF = true;
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "TEXTURED", "GLTF", "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
    lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));

    // Create Planes
    for (unsigned int i = 0; i < 3; i++) {
        m_Planes.addChild<Plane>(m_RendererLibrary->GetPlaneVBO(), m_RendererLibrary->GetPlaneVertices(), 48);
        lastEntity = m_Planes.children.back().get();
        lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 2.0f, (i * 3.0f)));
        lastEntity->transform.setLocalRotation(glm::vec3(90.0f, 0.0f, 0.0f));
        lastEntity->transform.setLocalScale(glm::vec3(2.0f));
        if (i == 0)
            lastEntity->material.setAlbedo(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
        else if (i == 1)
            lastEntity->material.setAlbedo(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
        else if (i == 2)
            lastEntity->material.setAlbedo(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
        lastEntity->material.setMetallic(1.0f);
        lastEntity->material.setRoughness(1.0f);
        lastEntity->material.setAO(1.0f);
        lastEntity->material.setEmissive(0.0f);
        lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
        m_ObjectsID++;
        lastEntity->material.setIsTextured(false);
        lastEntity->material.setIsTranslucent(true);
        lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
        lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
        lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
        lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
        lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
        lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
        ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "TRANSLUCENT", "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
        lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));
    }
}

void RendererLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.GetFov(), static_cast<float>(m_Width) / m_Height, m_Camera.GetNearPlane(), m_Camera.GetFarPlane());
    m_CamFrustum = m_Camera.CreateFrustumFromCamera(static_cast<float>(m_Width) / m_Height, glm::radians(m_Camera.GetFov()), m_Camera.GetNearPlane(), m_Camera.GetFarPlane());

    m_MatricesUBO.FillBuffer(&projView, 0, sizeof(glm::mat4));

    m_CameraUBO.FillBuffer(&m_Camera.GetViewMatrix(), 0, sizeof(glm::mat4));
    m_CameraUBO.FillBuffer(&m_Camera.GetPosition(), sizeof(glm::mat4), sizeof(glm::vec3));
    m_CameraUBO.FillBuffer(&m_Camera.GetFarPlane(), sizeof(glm::mat4) + sizeof(glm::vec3), sizeof(float));

    // Render Directional Lights Shadows
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        if (m_DirectionalLights[i]->GetCastShadows()) {
            // generate the depth map for directional shadows
            ResourceManager::GetShader("depth_map").Use().SetInteger("dirLightIndex", i);

            const auto lightMatrices = m_DirectionalLights[i]->GetLightSpaceMatrices();
            for (size_t j = 0; j < lightMatrices.size(); j++) {
                size_t offset = i * sizeof(glm::mat4x4) * 16 + j * sizeof(glm::mat4x4);
                m_LightsUBO.FillBuffer(&lightMatrices[j], offset, sizeof(glm::mat4x4));
            }

            // shadow mapping
            glViewport(0, 0, m_DirectionalLights[i]->GetDepthMapResolution(), m_DirectionalLights[i]->GetDepthMapResolution());

            // directional shadows
            m_DirectionalLights[i]->GetDepthMapFBO().Bind();
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            m_Models.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_map"));
            m_Cubes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_map"));
            m_Spheres.renderSceneGraphSimple(GL_TRIANGLE_STRIP, ResourceManager::GetShader("depth_map"));
            // m_Planes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_map"));
            glCullFace(GL_BACK);
            FrameBuffer::UnBind();

            glViewport(0, 0, m_Width, m_Height);
        }
    }

    // Render Point Lights Shadows
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        if (m_PointLights[i]->GetCastShadows()) {
            // generate depth cube map
            std::vector<glm::mat4> pointLightMatrices;
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            pointLightMatrices.push_back(m_PointLights[i]->GetPointLightProjection() * glm::lookAt(m_PointLights[i]->GetPosition(), m_PointLights[i]->GetPosition() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

            for (GLuint j = 0; j < 6; j++) {
                ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), pointLightMatrices[j]);
            }

            ResourceManager::GetShader("depth_cube_map").Use().SetVector3f("lightPos", m_PointLights[i]->GetPosition());
            if (m_DebugDepthCubeMap) {
                ResourceManager::GetShader("debug_depth_cube_map").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_PointLights[i]->GetPosition());
            }

            // shadow mapping
            glViewport(0, 0, m_PointLights[i]->GetDepthCubeMapResolution(), m_PointLights[i]->GetDepthCubeMapResolution());

            // point shadows
            m_PointLights[i]->GetDepthCubeMapFBO().Bind();
            glClear(GL_DEPTH_BUFFER_BIT);

            m_Models.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"));
            m_Cubes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"));
            m_Spheres.renderSceneGraphSimple(GL_TRIANGLE_STRIP, ResourceManager::GetShader("depth_cube_map"));
            // m_Planes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"));

            FrameBuffer::UnBind();

            glViewport(0, 0, m_Width, m_Height);
        }
    }

    // Render Normal Scene
    // opaque pass
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    m_MultisampleFBO.Bind();
    glClearColor(m_BackgroundColor.r, m_BackgroundColor.g, m_BackgroundColor.b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_UsePolygonLines) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // Render Point Lights Mesh
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        if (m_PointLights[i]->GetRenderMesh()) {
            m_PointLights[i]->RenderMesh();
        }
    }

    unsigned int total = 0, display = 0;

    if (m_UseSkybox) {
        m_PBR->GetIrradiancemap().Bind(GL_TEXTURE_CUBE_MAP, 0);
        m_PBR->GetPrefiltermap().Bind(GL_TEXTURE_CUBE_MAP, 1);
        m_PBR->GetBRDFLUTTexture().Bind(GL_TEXTURE_2D, 2);
    }
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        glActiveTexture(GL_TEXTURE9 + i);
        m_DirectionalLights[i]->GetDepthMapFBO().BindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        glActiveTexture(GL_TEXTURE19 + i);
        m_PointLights[i]->GetDepthCubeMapFBO().BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    if (m_DebugDepthCubeMap) {
        m_Models.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("debug_depth_cube_map"));
        m_Cubes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("debug_depth_cube_map"));
        m_Spheres.renderSceneGraphSimple(GL_TRIANGLE_STRIP, ResourceManager::GetShader("debug_depth_cube_map"));
        m_Planes.renderSceneGraphSimple(GL_TRIANGLES, ResourceManager::GetShader("debug_depth_cube_map"));
    }
    else {
        m_Models.renderSceneGraph(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);
        m_Cubes.renderSceneGraph(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);
        m_Spheres.renderSceneGraph(GL_TRIANGLE_STRIP, m_CamFrustum, m_ObjectUBO, display, total);
        m_Planes.renderSceneGraph(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);

        // GL_TRACE("Total process in CPU : {0} / Total send to GPU : {1}", total, display);

        if (m_UseSkybox) {
            m_PBR->RenderSkybox();
        }

        // translucent pass
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(2, GL_ONE, GL_ONE);
        glBlendFunci(3, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        glClearBufferfv(GL_COLOR, 2, &glm::vec4(0.0f)[0]);
        glClearBufferfv(GL_COLOR, 3, &glm::vec4(1.0f)[0]);

        if (m_UseSkybox) {
            m_PBR->GetIrradiancemap().Bind(GL_TEXTURE_CUBE_MAP, 0);
            m_PBR->GetPrefiltermap().Bind(GL_TEXTURE_CUBE_MAP, 1);
            m_PBR->GetBRDFLUTTexture().Bind(GL_TEXTURE_2D, 2);
        }
        m_Models.renderSceneGraphTranslucent(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);
        m_Cubes.renderSceneGraphTranslucent(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);
        m_Spheres.renderSceneGraphTranslucent(GL_TRIANGLE_STRIP, m_CamFrustum, m_ObjectUBO, display, total);
        m_Planes.renderSceneGraphTranslucent(GL_TRIANGLES, m_CamFrustum, m_ObjectUBO, display, total);
    }

    FrameBuffer::UnBind();

    // composite pass
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_MultisampleFBO.Bind();
    glActiveTexture(GL_TEXTURE2);
    m_MultisampleFBO.BindTranslucentTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glActiveTexture(GL_TEXTURE3);
    m_MultisampleFBO.BindTranslucentTexture(GL_TEXTURE_2D_MULTISAMPLE, 1);
    ResourceManager::GetShader("composite").Use();
    RenderQuad(GL_TRIANGLES);
    Texture2D::UnBind(GL_TEXTURE_2D_MULTISAMPLE);
    FrameBuffer::UnBind();

    // Reset polygon mode to fill
    if (m_UsePolygonLines) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_MultisampleFBO.Blit(m_PostProcessor->GetHDRFBO(), m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    if (m_PostProcessor->GetUseBloom()) {
        m_PostProcessor->RenderBloomTexture();
    }

    // Bind to ImGui Image
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
    glDisable(GL_BLEND);

    m_ImGUIFBO.Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_PostProcessor->RenderPostProcessingQuad();
    FrameBuffer::UnBind();

    // Bind to ImGui Debug Image
    if (m_UseDebugWindow) {
        m_DebugFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        if (m_DebugDepthMap) {
            m_DirectionalLights[m_DebugDepthMapIndex]->RenderDepthMapQuad();
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            m_PostProcessor->GetHDRFBO().BindTexture(GL_TEXTURE_2D, 0);
            ResourceManager::GetShader("debug_quad").Use();
            RenderQuad(GL_TRIANGLES);
            Texture2D::UnBind(GL_TEXTURE_2D);
        }
        FrameBuffer::UnBind();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
}

void RendererLayer::OnImGuiRender()
{
    Application& app = Application::Get();

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (m_UseDebugWindow) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Debug Image");

        ImGui::ImageButton((void*)(intptr_t)m_DebugFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0), 0);
        ImGui::PopStyleVar();

        ImGui::End();
    }

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
        m_Camera.SetFirstClick(true);
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
        ImGui::DragFloat3("Position", (float*)&m_Camera.GetPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::DragFloat3("Orientation", (float*)&m_Camera.GetOrientation(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::SliderFloat("Field of view", (float*)&m_Camera.GetFov(), 1.0f, 90.0f, "%.2f");
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Lighting", base_flags)) {
        for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
            m_DirectionalLights[i]->RenderGUI(i, m_LightsUBO);
        }

        for (size_t i = 0; i < m_PointLights.size(); i++) {
            m_PointLights[i]->RenderGUI(i, m_LightsUBO);
        }

        if (ImGui::Button("Add Light", ImVec2(0, 0))) {
            ImGui::OpenPopup("New Light");
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        static const char* items[] = { "Directional", "Point" };
        static int currentItem = 0;

        if (ImGui::BeginPopupModal("New Light", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Separator();
            ImGui::Text("Choose the type of light:");
            ImGui::Separator();
            ImGui::Combo("Light Type", &currentItem, items, IM_ARRAYSIZE(items));
            ImGui::Separator();

            if (ImGui::Button("Add")) {
                DirectionalLight* dirLight = nullptr;
                PointLight* pointLight = nullptr;

                switch (currentItem) {
                    case 0:
                        dirLight = new DirectionalLight(this);
                        AddLight(dirLight);
                        break;
                    case 1:
                        pointLight = new PointLight(this);
                        AddLight(pointLight);
                        break;
                }
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("3D Objects", base_flags)) {
        for (unsigned int i = 0; i < m_Planes.children.size(); i++) {
            m_Planes.children[i].get()->renderGUI(i, m_ObjectUBO);
        }
        for (unsigned int i = 0; i < m_Models.children.size(); i++) {
            m_Models.children[i].get()->renderGUI(i, m_ObjectUBO);
        }
        for (unsigned int i = 0; i < m_Cubes.children.size(); i++) {
            m_Cubes.children[i].get()->renderGUI(i, m_ObjectUBO);
        }
        for (unsigned int i = 0; i < m_Spheres.children.size(); i++) {
            m_Spheres.children[i].get()->renderGUI(i, m_ObjectUBO);
        }
        if (ImGui::Button("Add Object", ImVec2(0, 0))) {
            ImGui::OpenPopup("New Object");
        }

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        static const char* items[] = { "Plane", "Cube", "Sphere", "Model" };
        static int currentItem = 0;

        if (ImGui::BeginPopupModal("New Object", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Separator();
            ImGui::Text("Choose the type of object:");
            ImGui::Separator();
            ImGui::Combo("Object Type", &currentItem, items, IM_ARRAYSIZE(items));
            ImGui::Separator();

            if (currentItem == 3) {
                if (ImGui::Button("Open...")) {
                    // clang-format off
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseFileDlgKey",
                        "Choose File ",
                        ".gltf,.obj",
                        ".",
                        1, nullptr, ImGuiFileDialogFlags_Modal);
                    // clang-format on

                    // Always center this window when appearing
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                }

                // display
                if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk()) {
                        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();

                        std::size_t pos = file.find("assets");

                        if (pos != std::string::npos) {
                            m_ModelFilePath = file.substr(pos);
                        }
                        std::replace(m_ModelFilePath.begin(), m_ModelFilePath.end(), '\\', '/');
                        ResourceManager::LoadModel(m_ModelFilePath.c_str(), "3d_model");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

                ImGui::Text("%s", m_ModelFilePath.c_str());
            }

            if (ImGui::Button("Add")) {
                switch (currentItem) {
                    case 0:
                        AddPlane();
                        break;
                    case 1:
                        AddCube();
                        break;
                    case 2:
                        AddSphere();
                        break;
                    case 3:
                        if (m_ModelFilePath == "None") {
                            break;
                        }
                        AddModel();
                        break;
                }
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Background", base_flags)) {
        if (ImGui::TreeNodeEx("Use Skybox", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseSkybox)) {
                m_ObjectUBO.FillBuffer(&m_UseSkybox, 0, sizeof(float));
                if (m_UseSkybox) {
                    m_PBR->RenderSkyboxGUI();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Background Color", base_flags)) {
            ImGui::ColorEdit3("", (float*)&m_BackgroundColor);
            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Debug", base_flags)) {
        if (ImGui::TreeNodeEx("Debug Image Window", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseDebugWindow)) {
                if (m_UseDebugWindow) {
                    ImGui::Checkbox("Depth Map (Directional Light)", &m_DebugDepthMap);
                    if (m_DebugDepthMap) {
                        ImGui::SliderInt("Light Index", &m_DebugDepthMapIndex, 0, m_DirectionalLights.size() - 1);
                        m_DirectionalLights[m_DebugDepthMapIndex]->RenderDebugGUI();
                    }
                }
            }
            ImGui::TreePop();
        }

        ImGui::Checkbox("Wireframe", &m_UsePolygonLines);
        ImGui::Checkbox("Depth Cube Map (Point Lights)", &m_DebugDepthCubeMap);
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
        m_MultisampleFBO.ResizeTranslucentAttachments(GL_TEXTURE_2D_MULTISAMPLE, m_Width, m_Height);
        m_MultisampleFBO.ResizeRenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        m_ImGUIFBO.Bind();
        m_ImGUIFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        m_DebugFBO.Bind();
        m_DebugFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
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
    m_DebugFBO.Destroy();
    m_MultisampleFBO.Destroy();

    m_MatricesUBO.Destroy();
    m_LightsUBO.Destroy();
    m_CameraUBO.Destroy();
    m_ObjectUBO.Destroy();

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

void RendererLayer::AddPlane()
{
    m_Planes.addChild<Plane>(m_RendererLibrary->GetPlaneVBO(), m_RendererLibrary->GetPlaneVertices(), 48);
    Entity* lastEntity = m_Planes.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(1.0f);
    lastEntity->material.setEmissive(0.0f);
    lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
    m_ObjectsID++;
    lastEntity->material.setIsTextured(false);
    lastEntity->material.setIsTranslucent(false);
    lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
    lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
    lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
    lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
    lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
    lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
    lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));
}

void RendererLayer::AddSphere()
{
    m_Spheres.addChild<Sphere>(m_RendererLibrary->GetSphereVBO(), &m_RendererLibrary->GetSphereVertices()[0], m_RendererLibrary->GetSphereVertices().size(), m_RendererLibrary->GetSphereEBO(), m_RendererLibrary->GetSphereIndicesCount());
    Entity* lastEntity = m_Spheres.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(1.0f);
    lastEntity->material.setEmissive(0.0f);
    lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
    m_ObjectsID++;
    lastEntity->material.setIsTextured(false);
    lastEntity->material.setIsTranslucent(false);
    lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
    lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
    lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
    lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
    lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
    lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
    lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));
}

void RendererLayer::AddCube()
{
    m_Cubes.addChild<Cube>(m_RendererLibrary->GetCubeVBO(), m_RendererLibrary->GetCubeVertices(), 288);
    Entity* lastEntity = m_Cubes.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(1.0f);
    lastEntity->material.setEmissive(0.0f);
    lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
    m_ObjectsID++;
    lastEntity->material.setIsTextured(false);
    lastEntity->material.setIsTranslucent(false);
    lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
    lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
    lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
    lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
    lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
    lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
    lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));
}

void RendererLayer::AddModel()
{
    m_Models.addChild<ModelEntity>(ResourceManager::GetModel("3d_model"));
    Entity* lastEntity = m_Models.children.back().get();
    lastEntity->transform.setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    lastEntity->transform.setLocalScale(glm::vec3(1.0f));
    lastEntity->material.setAlbedo(glm::vec4(1.0f));
    lastEntity->material.setMetallic(1.0f);
    lastEntity->material.setRoughness(1.0f);
    lastEntity->material.setAO(0.20f);
    lastEntity->material.setEmissive(0.0f);
    lastEntity->material.fillMaterialBuffer(m_ObjectUBO, m_ObjectsID);
    m_ObjectsID++;
    lastEntity->material.setIsTextured(true);
    lastEntity->material.setIsTranslucent(false);
    lastEntity->material.setAlbedoTexture(ResourceManager::GetTexture("default_albedo"));
    lastEntity->material.setNormalTexture(ResourceManager::GetTexture("default_normal"));
    lastEntity->material.setMetallicTexture(ResourceManager::GetTexture("default_metallic"));
    lastEntity->material.setRoughnessTexture(ResourceManager::GetTexture("default_roughness"));
    lastEntity->material.setAOTexture(ResourceManager::GetTexture("default_ao"));
    lastEntity->material.setEmissiveTexture(ResourceManager::GetTexture("default_emissive"));
    lastEntity->isGLTF = true;
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", { "TEXTURED", "GLTF", "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" });
    lastEntity->material.setShader(ResourceManager::GetShader("pbr_lighting"));
}

void RendererLayer::AddLight(DirectionalLight* light)
{
    m_DirectionalLights.push_back(light);
    light->InitLightUBO(m_LightsUBO);
    for (size_t i = 0; i < m_DirectionalLights.size(); i++) {
        int dirLightOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + i * 2 * sizeof(glm::vec4);
        glm::vec3 lightColor = m_DirectionalLights[i]->GetColor() * m_DirectionalLights[i]->GetIntensity();

        m_LightsUBO.FillBuffer(&m_DirectionalLights[i]->GetDirection(), dirLightOffset, sizeof(glm::vec4));
        m_LightsUBO.FillBuffer(&lightColor, dirLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        int castShadows = m_DirectionalLights[i]->GetCastShadows();
        m_LightsUBO.FillBuffer(&castShadows, dirLightOffset + sizeof(glm::vec4) + sizeof(glm::vec3), sizeof(float));
    }
    int lightsOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS;
    size_t dirLightsSize = m_DirectionalLights.size();
    m_LightsUBO.FillBuffer(&dirLightsSize, lightsOffset, sizeof(float));
}

void RendererLayer::RemoveLight(DirectionalLight* light)
{
    m_DirectionalLights.erase(std::remove(m_DirectionalLights.begin(), m_DirectionalLights.end(), light), m_DirectionalLights.end());
    int lightsOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS;
    size_t dirLightsSize = m_DirectionalLights.size();
    m_LightsUBO.FillBuffer(&dirLightsSize, lightsOffset, sizeof(float));
}

void RendererLayer::AddLight(PointLight* light)
{
    m_PointLights.push_back(light);
    for (size_t i = 0; i < m_PointLights.size(); i++) {
        int pointLightOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + i * 2 * sizeof(glm::vec4);
        glm::vec3 lightColor = m_PointLights[i]->GetColor() * m_PointLights[i]->GetIntensity();

        m_LightsUBO.FillBuffer(&m_PointLights[i]->GetPosition(), pointLightOffset, sizeof(glm::vec4));
        m_LightsUBO.FillBuffer(&lightColor, pointLightOffset + sizeof(glm::vec4), sizeof(glm::vec3));
        int castShadows = m_PointLights[i]->GetCastShadows();
        m_LightsUBO.FillBuffer(&castShadows, pointLightOffset + sizeof(glm::vec4) + sizeof(glm::vec3), sizeof(float));
    }
    int lightsOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS;
    size_t pointLightsSize = m_PointLights.size();
    m_LightsUBO.FillBuffer(&pointLightsSize, lightsOffset + sizeof(float), sizeof(float));

    ResourceManager::GetShader("debug_depth_cube_map").Use().SetInteger("nrPointLights", m_PointLights.size());
    ResourceManager::GetShader("debug_depth_cube_map").Use().SetVector3f(("pointLights[" + std::to_string(m_PointLights.size()-1) + "].position").c_str(), light->GetPosition());
    ResourceManager::GetShader("debug_depth_cube_map").Use().SetFloat("far_plane", m_Camera.GetFarPlane());
    ResourceManager::GetShader("depth_cube_map").Use().SetFloat("far_plane", m_Camera.GetFarPlane());

    light->PointLightProjectionMatrix(m_Camera.GetNearPlane(), m_Camera.GetFarPlane());
}

void RendererLayer::RemoveLight(PointLight* light)
{
    m_PointLights.erase(std::remove(m_PointLights.begin(), m_PointLights.end(), light), m_PointLights.end());
    int lightsOffset = sizeof(glm::mat4x4) * 16 * DirectionalLight::MAX_LIGHTS + sizeof(glm::vec4) + 2 * sizeof(glm::vec4) * DirectionalLight::MAX_LIGHTS + 2 * sizeof(glm::vec4) * PointLight::MAX_LIGHTS;
    size_t pointLightsSize = m_PointLights.size();
    m_LightsUBO.FillBuffer(&pointLightsSize, lightsOffset + sizeof(float), sizeof(float));
}