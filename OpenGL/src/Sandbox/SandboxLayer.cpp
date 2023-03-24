#include "Sandbox/SandboxLayer.h"

#include "Core/Application.h"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
    Application &app = Application::Get();
    m_Window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    m_Camera = Camera(m_Width, m_Height, glm::vec3(0.0f, 2.0f, 20.0f));
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
    ResourceManager::LoadModel("assets/objects/biomutant/Biomech_Mutant_Skin_1.gltf", "3d_model");
    // Load Textures
    // Rusted Iron
    ResourceManager::LoadTexture("assets/textures/pbr/rusted_iron/albedo.png", "rusted_albedo", true);
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
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "light_source");
    ResourceManager::LoadShader("assets/shaders/postProcessingVS.glsl", "assets/shaders/postProcessingFS.glsl", nullptr, "post_proc");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedFS.glsl", nullptr, "pbr_lighting_textured");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/equirectangularToCubemapFS.glsl", nullptr, "equirectangular_to_cubemap");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/irradianceConvolutionFS.glsl", nullptr, "irradiance");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/prefiltermapFS.glsl", nullptr, "prefilter");
    ResourceManager::LoadShader("assets/shaders/hdrSkyboxVS.glsl", "assets/shaders/hdrSkyboxFS.glsl", nullptr, "hdr_skybox");
    ResourceManager::LoadShader("assets/shaders/brdfVS.glsl", "assets/shaders/brdfFS.glsl", nullptr, "brdf");
    ResourceManager::LoadShader("assets/shaders/blurVS.glsl", "assets/shaders/blurFS.glsl", nullptr, "blur");
    ResourceManager::LoadShader("assets/shaders/depthQuadVS.glsl", "assets/shaders/depthQuadFS.glsl", nullptr, "depth_quad");
    ResourceManager::LoadShader("assets/shaders/depthMapVS.glsl", "assets/shaders/depthMapFS.glsl", nullptr, "depth_map");
    ResourceManager::LoadShader("assets/shaders/depthCubeMapVS.glsl", "assets/shaders/depthCubeMapFS.glsl", "assets/shaders/depthCubeMapGS.glsl", "depth_cube_map");

    // Post Processing - Activate only one per group
    // Kernel effects
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
    // General Post Processing
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
    ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.bloom", m_UseBloom);
    ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.exposure", m_Exposure);

    // PBR Settings
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("material.albedo", m_Albedo);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.metallic", m_Metallic);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.roughness", m_Roughness);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.ao", m_AO);

    // Generate directional light
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.use", m_UseDirLight);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.direction", m_DirLightDirection);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.shadows", m_UseDirShadows);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("dirLight.use", m_UseDirLight);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("dirLight.direction", m_DirLightDirection);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("dirLight.shadows", m_UseDirShadows);

    // Generate point lights
    m_PointLightPositions.push_back(glm::vec3(-4.5f, 3.5f, 8.0f));
    m_PointLightColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    m_PointLightIntensities.push_back(100.0f);
    // m_PointLightPositions.push_back(glm::vec3(5.0f, 2.0f, 10.0f));
    // m_PointLightColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    // m_PointLightIntensities.push_back(100.0f);
    for (size_t i = 0; i < m_PointLightPositions.size(); i++) {
        ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_PointLightPositions[i]);
        ResourceManager::GetShader("pbr_lighting").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_PointLightColors[i] * m_PointLightIntensities[i]);
        ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_UsePointShadows);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), m_PointLightPositions[i]);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f(("pointLights[" + std::to_string(i) + "].color").c_str(), m_PointLightColors[i] * m_PointLightIntensities[i]);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_UsePointShadows);
    }
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("pointLight.use", m_UsePointLights);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("pointLight.use", m_UsePointLights);
    ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);

    multisampleFBO = FrameBuffer();
    multisampleFBO.Bind();
    multisampleFBO.TextureAttachment(2, 0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGB16F, m_Width, m_Height);
    multisampleFBO.RenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    hdrFBO = FrameBuffer();
    hdrFBO.Bind();
    hdrFBO.TextureAttachment(2, 0, GL_TEXTURE_2D, GL_RGB16F, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    for (GLuint i = 0; i < 2; i++) {
        pingpongFBO[i] = FrameBuffer();
        pingpongFBO[i].Bind();
        pingpongFBO[i].TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGB16F, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();
    }

    imguiFBO = FrameBuffer();
    imguiFBO.Bind();
    imguiFBO.TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    captureFBO = FrameBuffer();
    captureFBO.Bind();
    captureFBO.RenderBufferAttachment(GL_FALSE, GL_DEPTH24_STENCIL8, m_EnvCubemapWidth, m_EnvCubemapHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    depthMapFBO = FrameBuffer();
    depthMapFBO.Bind();
    depthMapFBO.TextureAttachment(1, 1, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    depthCubeMapFBO = FrameBuffer();
    depthCubeMapFBO.Bind();
    depthCubeMapFBO.TextureAttachment(1, 2, GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight);
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
    captureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_IrradiancemapWidth, m_IrradiancemapHeight);

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
        captureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, mipWidth, mipHeight);
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
    captureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUTTexture.ID, 0);

    glViewport(0, 0, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    ResourceManager::GetShader("brdf").Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    FrameBuffer::UnBind();

    glViewport(0, 0, m_Width, m_Height);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // generate depth map projection matrix
    float boxLimit = 20.0f;
    float minX = -boxLimit;
    float maxX = boxLimit;
    float minY = -boxLimit;
    float maxY = boxLimit;
    float minZ = -boxLimit;
    float maxZ = boxLimit;

    glm::vec3 center = glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);

    float width = maxX - minX;
    float height = maxY - minY;
    float depth = maxZ - minZ;

    float nearDistance = glm::length(center - m_DepthMapOrig) - depth / 2.0f;
    float farDistance = glm::length(center - m_DepthMapOrig) + depth / 2.0f;

    float left = center.x - width / 2.0f - m_DepthMapOrig.x;
    float right = center.x + width / 2.0f - m_DepthMapOrig.x;
    float bottom = center.y - height / 2.0f - m_DepthMapOrig.y;
    float top = center.y + height / 2.0f - m_DepthMapOrig.y;

    m_DepthMapProjection = glm::ortho(left, right, bottom, top, nearDistance, farDistance);

    ResourceManager::GetShader("depth_quad").Use().SetFloat("nearPlane", nearDistance);
    ResourceManager::GetShader("depth_quad").Use().SetFloat("farPlane", farDistance);

    // generate point light projection matrix
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspect_ratio = static_cast<float>(m_ShadowWidth) / static_cast<float>(m_ShadowHeight);
    m_PointLightProjection = glm::perspective(glm::radians(90.0f), aspect_ratio, nearPlane, farPlane);

    ResourceManager::GetShader("depth_cube_map").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("far_plane", farPlane);
    ResourceManager::GetShader("pbr_lighting_texture").Use().SetFloat("far_plane", farPlane);
}

void SandboxLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, static_cast<float>(m_Width) / m_Height, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("light_source").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(0, m_Camera.GetProjectionMatrix());
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(1, m_Camera.GetViewMatrix());

    glEnable(GL_DEPTH_TEST);

    if (m_UseDirShadows && m_UseDirLight) {
        // generate the depth map for directional shadows
        glm::mat4 depthMapView = glm::lookAt(m_DepthMapOrig, m_DepthMapOrig + m_DirLightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 depthMapLightSpaceMatrix = m_DepthMapProjection * depthMapView;

        ResourceManager::GetShader("depth_map").Use().SetMatrix4(0, depthMapLightSpaceMatrix);
        ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(2, depthMapLightSpaceMatrix);
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(2, depthMapLightSpaceMatrix);

        // shadow mapping
        glViewport(0, 0, m_ShadowWidth, m_ShadowHeight);

        // directional shadows
        depthMapFBO.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
        renderPlane();

        if (m_UsePointLights) {
            for (size_t i = 0; i < m_PointLightPositions.size(); i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, m_PointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
                ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
                renderCube();
            }
        }

        renderModel(ResourceManager::GetShader("depth_map"));

        for (GLuint i = 0; i < 5; i++) {
            for (GLuint j = 0; j < 5; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-6.0f + (i*3), 0.5f + (j*3), 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
                renderSphere();
            }
        }

        FrameBuffer::UnBind();

        glViewport(0, 0, m_Width, m_Height);
    }

    if (m_UsePointShadows && m_UsePointLights) {
        // generate depth cube map
        std::vector<glm::mat4> pointLightMatrices;

        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        pointLightMatrices.push_back(m_PointLightProjection * glm::lookAt(m_PointLightPositions[0], m_PointLightPositions[0] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        for (GLuint i = 0; i < 6; i++) {
            ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), pointLightMatrices[i]);
        }

        ResourceManager::GetShader("depth_cube_map").Use().SetVector3f("lightPos", m_PointLightPositions[0]);

        // shadow mapping
        glViewport(0, 0, m_ShadowWidth, m_ShadowHeight);

        // point shadows
        depthCubeMapFBO.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(1, model);
        renderPlane();

        renderModel(ResourceManager::GetShader("depth_cube_map"));

        for (GLuint i = 0; i < 5; i++) {
            for (GLuint j = 0; j < 5; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-6.0f + (i*3), 0.5f + (j*3), 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(1, model);
                renderSphere();
            }
        }

        FrameBuffer::UnBind();

        glViewport(0, 0, m_Width, m_Height);
    }

    // Render Normal Scene
    multisampleFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render Plane
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    // ResourceManager::GetTexture("grass_albedo").Bind(3);
    // ResourceManager::GetTexture("grass_normal").Bind(4);
    // ResourceManager::GetTexture("grass_metallic").Bind(5);
    // ResourceManager::GetTexture("grass_roughness").Bind(6);
    // ResourceManager::GetTexture("grass_ao").Bind(7);
    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE8);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE9);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(1, model);
    renderPlane();
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Light Cubes
    if (m_UsePointLights) {
        for (size_t i = 0; i < m_PointLightPositions.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m_PointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            ResourceManager::GetShader("light_source").Use().SetMatrix4(1, model);
            ResourceManager::GetShader("light_source").Use().SetVector3f("color", m_PointLightColors[i] * m_PointLightIntensities[i]);
            renderCube();
        }
    }

    // Render Models
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE8);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE9);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    renderModel(ResourceManager::GetShader("pbr_lighting"));
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Spheres
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE8);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE9);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    for (GLuint i = 0; i < 5; i++) {
        for (GLuint j = 0; j < 5; j++) {
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

    FrameBuffer::UnBind();

    // Bloom
    if (m_UseBloom) {
        // blur bright fragments with two-pass Gaussian blur
        bool horizontal = true, first_iteration = true;
        GLuint amount = 10;
        for (GLuint i = 0; i < amount; i++) {
            pingpongFBO[horizontal].Bind();
            ResourceManager::GetShader("blur").Use().SetInteger("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            if (first_iteration)
                hdrFBO.BindTexture(GL_TEXTURE_2D, 1);
            else
                pingpongFBO[!horizontal].BindTexture(GL_TEXTURE_2D, 0);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        FrameBuffer::UnBind();
    }

    multisampleFBO.Blit(hdrFBO, m_Width, m_Height);
    FrameBuffer::UnBind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    imguiFBO.Bind();
    if (m_DebugDepthMap) {
        glActiveTexture(GL_TEXTURE0);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
        ResourceManager::GetShader("depth_quad").Use();
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        hdrFBO.BindTexture(GL_TEXTURE_2D, 0);
        if (m_UseBloom) {
            glActiveTexture(GL_TEXTURE1);
            pingpongFBO[0].BindTexture(GL_TEXTURE_2D, 0);
        }
        ResourceManager::GetShader("post_proc").Use();
    }
    renderQuad();
    Texture2D::UnBind();
    FrameBuffer::UnBind();
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
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 4.0f));
    model = glm::scale(model, glm::vec3(1.0f));
    shader.Use().SetMatrix4(1, model);
    ResourceManager::GetModel("3d_model").Draw(shader);
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

    ImGui::ImageButton((void*)(intptr_t)imguiFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0), 0);
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

    ImGui::Begin("Metrics");
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());

#ifdef GL_DEBUG
    ImGui::Text("Running on Debug mode");
#elif GL_RELEASE
    ImGui::Text("Running on Release mode");
#elif GL_DIST
    ImGui::Text("Running on Dist mode");
#endif

    ImGui::Text("Application average\n %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();

    ImGui::Begin("Opions");

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::CollapsingHeader("Camera", base_flags)) {
        ImGui::DragFloat3("Position", (float*)&m_Camera.m_Position, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::DragFloat3("Orientation", (float*)&m_Camera.m_Orientation, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::SliderFloat("Field of view", &m_Camera.m_Fov, 1.0f, 90.0f, "%.2f");
    }

    if (ImGui::CollapsingHeader("Post Processing", base_flags)) {
        if (ImGui::TreeNodeEx("Kernel Effects", base_flags)) {
            if (ImGui::Checkbox("Blur", &m_UseBlur)) {
                m_UseEdge = false;
                m_UseRidge = false;
                m_UseSharpen = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            if (ImGui::Checkbox("Sharpen", &m_UseSharpen)) {
                m_UseEdge = false;
                m_UseRidge = false;
                m_UseBlur = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            if (ImGui::Checkbox("Ridge", &m_UseRidge)) {
                m_UseEdge = false;
                m_UseSharpen = false;
                m_UseBlur = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.blur", m_UseBlur);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.sharpen", m_UseSharpen);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.ridge", m_UseRidge);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.edge", m_UseEdge);
            }
            if (ImGui::Checkbox("Edge", &m_UseEdge)) {
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

        if (ImGui::TreeNodeEx("General Post Processing Effects", base_flags)) {
            if (ImGui::Checkbox("Greyscale", &m_UseGreyscale)) {
                m_UseInversion = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
            }
            if (ImGui::Checkbox("Inversion", &m_UseInversion)) {
                m_UseGreyscale = false;
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.greyscale", m_UseGreyscale);
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.inversion", m_UseInversion);
            }
            if (ImGui::Checkbox("Bloom", &m_UseBloom)) {
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.bloom", m_UseBloom);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("HDR Settings", base_flags)) {
            if (ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.exposure", m_Exposure);
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("PBR Settings", base_flags)) {
        if (ImGui::ColorEdit3("Albedo", (float*)&m_Albedo)) {
            ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("material.albedo", m_Albedo);
        }
        if (ImGui::SliderFloat("Metallic", &m_Metallic, 0.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.metallic", m_Metallic);
        }
        if (ImGui::SliderFloat("Roughness", &m_Roughness, 0.0f, 1.0f, "%.2f")) {
            ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.roughness", m_Roughness);
        }
    }

    if (ImGui::CollapsingHeader("Light Settings", base_flags)) {
        if (ImGui::TreeNodeEx("Directional Light", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseDirLight)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.use", m_UseDirLight);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("dirLight.use", m_UseDirLight);
            }
            if (ImGui::SliderFloat3("Direction", (float*)&m_DirLightDirection, -1.0f, 1.0f, "%.2f")) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.direction", m_DirLightDirection);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("dirLight.direction", m_DirLightDirection);
            }
            if (ImGui::ColorEdit3("Color", (float*)&m_DirLightColor)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
            }
            if (ImGui::DragFloat("Intensity", &m_DirLightIntensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("dirLight.color", m_DirLightColor * m_DirLightIntensity);
            }
            if (ImGui::Checkbox("Directional Shadows", &m_UseDirShadows)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.shadows", m_UseDirShadows);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("dirLight.shadows", m_UseDirShadows);
            }
            if (ImGui::Checkbox("Debug Depth Map", &m_DebugDepthMap))
            {}

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Point Lights", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UsePointLights)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("pointLight.use", m_UsePointLights);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("pointLight.use", m_UsePointLights);
            }
            if (ImGui::DragFloat3("Position", (float*)&m_PointLightPositions[0], 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("pointLights[0].position", m_PointLightPositions[0]);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("pointLights[0].position", m_PointLightPositions[0]);
            }
            if (ImGui::ColorEdit3("Color", (float*)&m_PointLightColors[0])) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("pointLights[0].color", m_PointLightColors[0] * m_PointLightIntensities[0]);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("pointLights[0].color", m_PointLightColors[0] * m_PointLightIntensities[0]);
            }
            if (ImGui::DragFloat("Intensity", &m_PointLightIntensities[0], 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("pointLights[0].color", m_PointLightColors[0] * m_PointLightIntensities[0]);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("pointLights[0].color", m_PointLightColors[0] * m_PointLightIntensities[0]);
            }
            if (ImGui::Checkbox("Point Shadows", &m_UsePointShadows)) {
                for (size_t i = 0; i < m_PointLightPositions.size(); i++) {
                    ResourceManager::GetShader("pbr_lighting").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_UsePointShadows);
                    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger(("pointLights[" + std::to_string(i) + "].shadows").c_str(), m_UsePointShadows);
                }
            }
            if (ImGui::Checkbox("Debug Depth Cube Map", &m_DebugDepthCubeMap)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
            }

            ImGui::TreePop();
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
        multisampleFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGB16F, m_Width, m_Height);
        multisampleFBO.ResizeRenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        hdrFBO.Bind();
        hdrFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGB16F, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        for (GLuint i = 0; i < 2; i++) {
            pingpongFBO[i].Bind();
            pingpongFBO[i].ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGB16F, m_Width, m_Height);
            FrameBuffer::CheckStatus();
            FrameBuffer::UnBind();
        }

        imguiFBO.Bind();
        imguiFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
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
    hdrFBO.Destroy();
    for (GLuint i = 0; i < 2; i++)
        pingpongFBO[i].Destroy();
    imguiFBO.Destroy();
    captureFBO.Destroy();
    depthMapFBO.Destroy();
    depthCubeMapFBO.Destroy();
}
