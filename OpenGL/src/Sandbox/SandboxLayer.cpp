#include "Sandbox/SandboxLayer.h"

#include "Core/Application.h"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    m_Camera = Camera(m_Width, m_Height, glm::vec3(0.0f, 2.0f, 5.0f));
}

void SandboxLayer::OnAttach()
{
    float cubeVertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };
    cubeVBO = VertexBuffer(&cubeVertices, sizeof(cubeVertices), GL_STATIC_DRAW);

    // sphere
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;

    const GLuint X_SEGMENTS = 64;
    const GLuint Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (GLuint x = 0; x <= X_SEGMENTS; ++x)
    {
        for (GLuint y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (GLuint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (GLuint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<GLuint>(indices.size());

    std::vector<float> data;
    for (size_t i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    sphereVBO = VertexBuffer(&data[0], data.size() * sizeof(float), GL_STATIC_DRAW);
    sphereEBO = IndexBuffer(&indices[0], indices.size() * sizeof(GLuint), GL_STATIC_DRAW);

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

    // Load Resources
    // Load Models
    ResourceManager::LoadModel("assets/objects/backpack/backpack.obj", "backpack");
    // Load Textures
    // Rusted Iron
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/albedo.png", "rusted_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/normal.png", "rusted_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/metallic.png", "rusted_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/roughness.png", "rusted_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/ao.png", "rusted_ao");
    // Grass
    ResourceManager::LoadTexture("assets/textures/pbr/grass/albedo.png", "grass_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/grass/normal.png", "grass_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/grass/metallic.png", "grass_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/grass/roughness.png", "grass_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/grass/ao.png", "grass_ao");
    // Load HDR Textures
    ResourceManager::LoadHDRTexture("assets/textures/hdr/Nature_8K_hdri.jpg", "skybox_hdr");
    // Load Shaders
    ResourceManager::LoadShader("assets/shaders/lightingVS.glsl", "assets/shaders/lightingFS.glsl", nullptr, "basic_lighting");
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "light_source");
    ResourceManager::LoadShader("assets/shaders/postProcessingVS.glsl", "assets/shaders/postProcessingFS.glsl", nullptr, "post_proc");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedFS.glsl", nullptr, "pbr_lighting_textured");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/equirectangularToCubemapFS.glsl", nullptr, "equirectangular_to_cubemap");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/irradianceConvolutionFS.glsl", nullptr, "irradiance");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/prefiltermapFS.glsl", nullptr, "prefilter");
    ResourceManager::LoadShader("assets/shaders/hdrSkyboxVS.glsl", "assets/shaders/hdrSkyboxFS.glsl", nullptr, "hdr_skybox");
    ResourceManager::LoadShader("assets/shaders/brdfVS.glsl", "assets/shaders/brdfFS.glsl", nullptr, "brdf");

    // Post Processing - Activate only one per group
    // Kernel effects
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
    // General Post Processing
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);

    // PBR Settings
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("material.albedo", m_Albedo);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.metallic", m_Metallic);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.roughness", m_Roughness);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.ao", m_AO);

    // Generate lights
    lightPositions.push_back(glm::vec3(-5.0f, 2.0f, 10.0f));
    lightPositions.push_back(glm::vec3(5.0f, 2.0f, 10.0f));
    lightColors.push_back(glm::vec3(150.0f, 150.0f, 150.0f));
    lightColors.push_back(glm::vec3(150.0f, 150.0f, 150.0f));
    for (size_t i = 0; i < lightPositions.size(); i++) {
        ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("lightPositions[" + std::to_string(i) + "]").c_str(), lightPositions[i]);
        ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("lightColors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("lightPositions[" + std::to_string(i) + "]").c_str(), lightPositions[i]);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("lightColors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
    }

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

    imguiFBO = FrameBuffer();
    imguiFBO.Bind();
    imguiFBO.TextureAttachment(1, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    captureFBO = FrameBuffer();
    captureFBO.Bind();
    captureFBO.RenderBufferAttachment(GL_FALSE, m_EnvCubemapWidth, m_EnvCubemapHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    // create environment cubemap
    m_EnvCubemap.Internal_Format = GL_RGB16F;
    m_EnvCubemap.Data_Type = GL_FLOAT;
    m_EnvCubemap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_EnvCubemap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_EnvCubemap.Wrap_R = GL_CLAMP_TO_EDGE;
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
    ResourceManager::GetTexture("skybox_hdr").Bind(0);
    ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(0, captureProjection);
    glViewport(0, 0, m_EnvCubemapWidth, m_EnvCubemapHeight);
    captureFBO.Bind();
    for (GLuint i = 0; i < 6; i++) {
        ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(1, captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap.ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    FrameBuffer::UnBind();
    Texture2D::UnBind();

    // create an irradiance cubemap, and re-scale captureFBO to irradiance scale
    m_Irradiancemap.Internal_Format = GL_RGB16F;
    m_Irradiancemap.Data_Type = GL_FLOAT;
    m_Irradiancemap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.GenerateCubemap(m_IrradiancemapWidth, m_IrradiancemapHeight);

    captureFBO.Bind();
    captureFBO.ResizeRenderBuffer(m_IrradiancemapWidth, m_IrradiancemapHeight);

    m_EnvCubemap.BindCubemap(0);
    ResourceManager::GetShader("irradiance").Use().SetMatrix4(0, captureProjection);
    glViewport(0, 0, m_IrradiancemapWidth, m_IrradiancemapHeight);
    for (GLuint i = 0; i < 6; i++) {
        ResourceManager::GetShader("irradiance").Use().SetMatrix4(1, captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_Irradiancemap.ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    FrameBuffer::UnBind();
    Texture2D::UnBindCubemap();

    // prefilter HDR environment map
    m_Prefiltermap.Internal_Format = GL_RGB16F;
    m_Prefiltermap.Data_Type = GL_FLOAT;
    m_Prefiltermap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_Prefiltermap.Filter_Min = GL_LINEAR_MIPMAP_LINEAR;
    m_Prefiltermap.GenerateCubemap(m_PrefiltermapWidth, m_PrefiltermapHeight, GL_TRUE);

    // run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    ResourceManager::GetShader("prefilter").Use().SetMatrix4(0, captureProjection);
    m_EnvCubemap.BindCubemap(0);
    captureFBO.Bind();
    GLuint maxMipLevels = 5;
    for (GLuint mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        GLuint mipWidth = m_PrefiltermapWidth * std::pow(0.5, mip);
        GLuint mipHeight = m_PrefiltermapHeight * std::pow(0.5, mip);
        captureFBO.ResizeRenderBuffer(mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

        ResourceManager::GetShader("prefilter").Use().SetFloat("roughness", roughness);
        for (GLuint i = 0; i < 6; ++i)
        {
            ResourceManager::GetShader("prefilter").Use().SetMatrix4(1, captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_Prefiltermap.ID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
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
    m_BRDFLUTTexture.Generate(m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight, 0);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader
    captureFBO.Bind();
    captureFBO.ResizeRenderBuffer(m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUTTexture.ID, 0);

    glViewport(0, 0, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    ResourceManager::GetShader("brdf").Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    FrameBuffer::UnBind();

    glViewport(0, 0, m_Width, m_Height);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SandboxLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, static_cast<float>(m_Width) / m_Height, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("basic_lighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("light_source").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(0, m_Camera.GetProjectionMatrix());
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(1, m_Camera.GetViewMatrix());

    glEnable(GL_DEPTH_TEST);

    multisampleFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render Plane
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    ResourceManager::GetTexture("grass_albedo").Bind(3);
    ResourceManager::GetTexture("grass_normal").Bind(4);
    ResourceManager::GetTexture("grass_metallic").Bind(5);
    ResourceManager::GetTexture("grass_roughness").Bind(6);
    ResourceManager::GetTexture("grass_ao").Bind(7);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
    renderPlane();
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Light Cubes
    for (size_t i = 0; i < lightPositions.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        ResourceManager::GetShader("light_source").Use().SetMatrix4(1, model);
        ResourceManager::GetShader("light_source").Use().SetVector3f("color", lightColors[i]);
        renderCube();
    }

    // Render Models
    // renderModel(ResourceManager::GetShader("basic_lighting"));

    // Render Spheres
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    for (unsigned int i = 0; i < 5; i++) {
        for (unsigned int j = 0; j < 5; j++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-6.0f + (i*3), 0.5f + (j*3), 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(1, model);
            renderSphere();
        }
    }
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Skybox
    m_EnvCubemap.BindCubemap(0);
    // m_Irradiancemap.BindCubemap(0);
    // m_Prefiltermap.BindCubemap(0);
    ResourceManager::GetShader("hdr_skybox").Use();
    renderCube();
    Texture2D::UnBindCubemap();

    multisampleFBO.Blit(intermediateFBO, m_Width, m_Height);
    multisampleFBO.UnBind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    imguiFBO.Bind();
    glActiveTexture(GL_TEXTURE0);
    intermediateFBO.BindTexture(0);
    ResourceManager::GetShader("post_proc").Use();
    renderQuad();
    Texture2D::UnBind();
    imguiFBO.UnBind();
}

void SandboxLayer::renderPlane()
{
    planeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    planeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    planeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVBO.UnlinkAttrib(0);
    planeVBO.UnlinkAttrib(1);
    planeVBO.UnlinkAttrib(2);
}

void SandboxLayer::renderCube()
{
    cubeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    cubeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    cubeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeVBO.UnlinkAttrib(0);
    cubeVBO.UnlinkAttrib(1);
    cubeVBO.UnlinkAttrib(2);
}

void SandboxLayer::renderSphere()
{
    sphereEBO.Bind();
    sphereVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    sphereVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    sphereVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    sphereVBO.UnlinkAttrib(0);
    sphereVBO.UnlinkAttrib(1);
    sphereVBO.UnlinkAttrib(2);
    sphereEBO.UnBind();
}

void SandboxLayer::renderQuad()
{
    screenQuadVBO.LinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    screenQuadVBO.LinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    screenQuadVBO.UnlinkAttrib(0);
    screenQuadVBO.UnlinkAttrib(1);
}

void SandboxLayer::renderModel(Shader shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.Use().SetMatrix4(1, model);
    ResourceManager::GetModel("backpack").Draw(shader);
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

    ImGui::Image((void*)(intptr_t)imguiFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::PopStyleVar();

    if (ImGui::IsWindowHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.MouseWheel) {
            m_Camera.ProcessMouseScroll(io.MouseWheel);
            GL_TRACE("FoV is {0} degrees", m_Camera.m_Fov);
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

    ImGui::Begin("Metrics");
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
    ImGui::Text("Application average\n %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();

    ImGui::Begin("Opions");

    if (ImGui::CollapsingHeader("Post Processing")) {
        if (ImGui::TreeNode("Kernel Effects")) {
            if (ImGui::Checkbox("Blur", &m_UseBlur)) {
                m_UseEdge = false;
                m_UseRidge = false;
                m_UseSharpen = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            else if (ImGui::Checkbox("Sharpen", &m_UseSharpen)) {
                m_UseEdge = false;
                m_UseRidge = false;
                m_UseBlur = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            else if (ImGui::Checkbox("Ridge", &m_UseRidge)) {
                m_UseEdge = false;
                m_UseSharpen = false;
                m_UseBlur = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            else if (ImGui::Checkbox("Edge", &m_UseEdge)) {
                m_UseRidge = false;
                m_UseSharpen = false;
                m_UseBlur = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("General Post Processing Effects")) {
            if (ImGui::Checkbox("Greyscale", &m_UseGreyscale)) {
                m_UseInversion = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
            }
            else if (ImGui::Checkbox("Inversion", &m_UseInversion)) {
                m_UseGreyscale = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("PBR Settings")) {
        if (ImGui::ColorEdit3("Albedo", (float*)&m_Albedo)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("material.albedo", m_Albedo);
        }
        else if (ImGui::DragFloat("Metallic", &m_Metallic, 0.01f, 0.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.metallic", m_Metallic);
        }
        else if (ImGui::DragFloat("Roughness", &m_Roughness, 0.01f, 0.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.roughness", m_Roughness);
        }
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

        imguiFBO.Bind();
        imguiFBO.ResizeTextureAttachment(GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
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
    cubeVBO.Destroy();
    sphereVBO.Destroy();
    sphereEBO.Destroy();
    screenQuadVBO.Destroy();

    multisampleFBO.Destroy();
    intermediateFBO.Destroy();
    imguiFBO.Destroy();
    captureFBO.Destroy();
}
