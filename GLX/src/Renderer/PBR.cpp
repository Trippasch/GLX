#include "Renderer/PBR.h"

PBR::PBR(RendererLayer* renderer)
    : m_Renderer(renderer)
{
    // Skybox
    ResourceManager::LoadHDRTexture(m_SkyboxFilename.c_str(), "hdr_texture");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/equirectangularToCubemapFS.glsl", nullptr, "equirectangular_to_cubemap");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/irradianceConvolutionFS.glsl", nullptr, "irradiance");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/prefiltermapFS.glsl", nullptr, "prefilter");
    ResourceManager::LoadShader("assets/shaders/hdrSkyboxVS.glsl", "assets/shaders/hdrSkyboxFS.glsl", nullptr, "hdr_skybox");
    ResourceManager::LoadShader("assets/shaders/brdfVS.glsl", "assets/shaders/brdfFS.glsl", nullptr, "brdf");

    // Lighting
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedFS.glsl", nullptr, "pbr_lighting_textured");
    ResourceManager::GetShader("pbr_lighting").Use().SetBlockIndex("Matrices", 0);

    // PBR Object Settings
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("object.emissiveIntensity", 1.0f);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("object.useIBL", 1);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("object.emissiveIntensity", 1.0f);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("object.useIBL", 1);

    m_CaptureFBO.Bind();
    m_CaptureFBO.RenderBufferAttachment(GL_FALSE, GL_DEPTH24_STENCIL8, m_EnvCubemapWidth, m_EnvCubemapHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();
}

PBR::~PBR()
{
    m_CaptureFBO.Destroy();

    m_EnvCubemap.Destroy();
    m_Irradiancemap.Destroy();
    m_Prefiltermap.Destroy();
    m_BRDFLUTTexture.Destroy();
}

void PBR::RenderSkybox()
{
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(0, m_Renderer->GetCamera().GetProjectionMatrix());
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(1, m_Renderer->GetCamera().GetViewMatrix());

    m_EnvCubemap.BindCubemap(0);
    // m_Irradiancemap.BindCubemap(0);
    // m_Prefiltermap.BindCubemap(0);
    ResourceManager::GetShader("hdr_skybox").Use();
    glCullFace(GL_FRONT);
    m_Renderer->RenderCube(GL_TRIANGLES);
    glCullFace(GL_BACK);
    Texture2D::UnBindCubemap();
}

void PBR::RenderSkyboxGUI()
{
    if (ImGui::Button("Open...")) {
        // clang-format off
        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseFileDlgKey",
            "Choose File ",
            ".jpg,.jpeg,.png,.hdr",
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
            m_SkyboxFilename = ("assets/textures/hdr-skyboxes/" + ImGuiFileDialog::Instance()->GetCurrentFileName());

            ResourceManager::LoadHDRTexture(m_SkyboxFilename.c_str(), "hdr_texture");
            m_EnvCubemap.Destroy();
            m_Irradiancemap.Destroy();
            m_Prefiltermap.Destroy();
            m_BRDFLUTTexture.Destroy();
            m_CaptureFBO.Bind();
            m_CaptureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_EnvCubemapWidth, m_EnvCubemapHeight);
            FrameBuffer::CheckStatus();
            FrameBuffer::UnBind();
            CreateSkybox();
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Text("%s", m_SkyboxFilename.c_str());
}

void PBR::CreateSkybox()
{
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // create environment cubemap
    m_EnvCubemap.Internal_Format = GL_RGB16F;
    m_EnvCubemap.Image_Format = GL_RGB;
    m_EnvCubemap.Data_Type = GL_FLOAT;
    m_EnvCubemap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_EnvCubemap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_EnvCubemap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_EnvCubemap.Filter_Min = GL_LINEAR_MIPMAP_LINEAR;
    m_EnvCubemap.Filter_Max = GL_LINEAR;
    m_EnvCubemap.GenerateCubemap(m_EnvCubemapWidth, m_EnvCubemapHeight);

    // projView matrices form capturing data onto the 6 cubemap face directions
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    ResourceManager::GetTexture("hdr_texture").Bind(0);
    ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(0, captureProjection);
    glViewport(0, 0, m_EnvCubemapWidth, m_EnvCubemapHeight);
    m_CaptureFBO.Bind();
    for (GLuint i = 0; i < 6; i++) {
        ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(1, captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap.ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Renderer->RenderCube(GL_TRIANGLES);
    }
    FrameBuffer::UnBind();
    Texture2D::UnBind();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap.ID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    Texture2D::UnBindCubemap();

    // create an irradiance cubemap, and re-scale m_CaptureFBO to irradiance scale
    m_Irradiancemap.Internal_Format = GL_RGB16F;
    m_Irradiancemap.Image_Format = GL_RGB;
    m_Irradiancemap.Data_Type = GL_FLOAT;
    m_Irradiancemap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Filter_Min = GL_LINEAR;
    m_Irradiancemap.Filter_Max = GL_LINEAR;
    m_Irradiancemap.GenerateCubemap(m_IrradiancemapWidth, m_IrradiancemapHeight);

    m_CaptureFBO.Bind();
    m_CaptureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_IrradiancemapWidth, m_IrradiancemapHeight);

    m_EnvCubemap.BindCubemap(0);
    ResourceManager::GetShader("irradiance").Use().SetMatrix4(0, captureProjection);
    glViewport(0, 0, m_IrradiancemapWidth, m_IrradiancemapHeight);
    for (GLuint i = 0; i < 6; i++) {
        ResourceManager::GetShader("irradiance").Use().SetMatrix4(1, captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_Irradiancemap.ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Renderer->RenderCube(GL_TRIANGLES);
    }
    FrameBuffer::UnBind();
    Texture2D::UnBindCubemap();

    // prefilter HDR environment map
    m_Prefiltermap.Internal_Format = GL_RGB16F;
    m_Prefiltermap.Image_Format = GL_RGB;
    m_Prefiltermap.Data_Type = GL_FLOAT;
    m_Prefiltermap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Filter_Min = GL_LINEAR_MIPMAP_LINEAR;
    m_Prefiltermap.Filter_Max = GL_LINEAR;
    m_Prefiltermap.GenerateCubemap(m_PrefiltermapWidth, m_PrefiltermapHeight, GL_TRUE);

    // run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    ResourceManager::GetShader("prefilter").Use().SetMatrix4(0, captureProjection);
    m_EnvCubemap.BindCubemap(0);
    m_CaptureFBO.Bind();
    GLuint maxMipLevels = 5;
    for (GLuint mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
        GLuint mipWidth = m_PrefiltermapWidth * std::pow(0.5, mip);
        GLuint mipHeight = m_PrefiltermapHeight * std::pow(0.5, mip);
        m_CaptureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

        ResourceManager::GetShader("prefilter").Use().SetFloat("roughness", roughness);
        for (GLuint i = 0; i < 6; ++i)
        {
            ResourceManager::GetShader("prefilter").Use().SetMatrix4(1, captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_Prefiltermap.ID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_Renderer->RenderCube(GL_TRIANGLES);
        }
    }
    FrameBuffer::UnBind();
    Texture2D::UnBindCubemap();

    // generate a 2D LUT from the BRDF equations used.
    m_BRDFLUTTexture.Internal_Format = GL_RG16F;
    m_BRDFLUTTexture.Image_Format = GL_RG;
    m_BRDFLUTTexture.Data_Type = GL_FLOAT;
    m_BRDFLUTTexture.Wrap_S = GL_CLAMP_TO_EDGE;
    m_BRDFLUTTexture.Wrap_T = GL_CLAMP_TO_EDGE;
    m_BRDFLUTTexture.Filter_Min = GL_LINEAR;
    m_BRDFLUTTexture.Filter_Max = GL_LINEAR;
    m_BRDFLUTTexture.Generate(m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight, 0);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader
    m_CaptureFBO.Bind();
    m_CaptureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUTTexture.ID, 0);

    glViewport(0, 0, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    ResourceManager::GetShader("brdf").Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Renderer->RenderQuad(GL_TRIANGLES);
    FrameBuffer::UnBind();

    glViewport(0, 0, m_Renderer->GetWidth(), m_Renderer->GetHeight());
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
}