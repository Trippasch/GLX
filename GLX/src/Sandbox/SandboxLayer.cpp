#include "Sandbox/SandboxLayer.h"

#include "Core/Application.h"

#include "Utils/glfw_tools.h"
#include "Utils/gl_tools.h"

#include "ImGui/ImGuiFileDialog.h"

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

    std::vector<float> sphereData;
    for (size_t i = 0; i < positions.size(); ++i)
    {
        sphereData.push_back(positions[i].x);
        sphereData.push_back(positions[i].y);
        sphereData.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            sphereData.push_back(normals[i].x);
            sphereData.push_back(normals[i].y);
            sphereData.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            sphereData.push_back(uv[i].x);
            sphereData.push_back(uv[i].y);
        }
    }
    sphereVBO = VertexBuffer(&sphereData[0], sphereData.size() * sizeof(float), GL_STATIC_DRAW);
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
    ResourceManager::LoadModel("assets/objects/helmet/DamagedHelmet.gltf", "helmet");
    ResourceManager::LoadModel("assets/objects/arrow/simple_arrow.gltf", "gizmo_arrow");
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
    // Wall
    ResourceManager::LoadTexture("assets/textures/pbr/wall/albedo.png", "wall_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/wall/normal.png", "wall_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/wall/metallic.png", "wall_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/wall/roughness.png", "wall_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/wall/ao.png", "wall_ao");
    // Gold
    ResourceManager::LoadTexture("assets/textures/pbr/gold/albedo.png", "gold_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/gold/normal.png", "gold_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/gold/metallic.png", "gold_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/gold/roughness.png", "gold_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/gold/ao.png", "gold_ao");
    // Plastic
    ResourceManager::LoadTexture("assets/textures/pbr/plastic/albedo.png", "plastic_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/plastic/normal.png", "plastic_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/plastic/metallic.png", "plastic_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/plastic/roughness.png", "plastic_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/plastic/ao.png", "plastic_ao");
    // // Patchy Meadow 1
    // ResourceManager::LoadTexture("assets/textures/pbr/patchy-meadow/patchy-meadow1_albedo.png", "patchy_albedo");
    // ResourceManager::LoadTexture("assets/textures/pbr/patchy-meadow/patchy-meadow1_normal-ogl.png", "patchy_normal");
    // ResourceManager::LoadTexture("assets/textures/pbr/patchy-meadow/patchy-meadow1_metallic.png", "patchy_metallic");
    // ResourceManager::LoadTexture("assets/textures/pbr/patchy-meadow/patchy-meadow1_roughness.png", "patchy_roughness");
    // ResourceManager::LoadTexture("assets/textures/pbr/patchy-meadow/patchy-meadow1_ao.png", "patchy_ao");
    // // Wavy Sand
    // ResourceManager::LoadTexture("assets/textures/pbr/wavy-sand/wavy-sand_albedo.png", "wavy_albedo");
    // ResourceManager::LoadTexture("assets/textures/pbr/wavy-sand/wavy-sand_normal-ogl.png", "wavy_normal");
    // ResourceManager::LoadTexture("assets/textures/pbr/wavy-sand/wavy-sand_metallic.png", "wavy_metallic");
    // ResourceManager::LoadTexture("assets/textures/pbr/wavy-sand/wavy-sand_roughness.png", "wavy_roughness");
    // ResourceManager::LoadTexture("assets/textures/pbr/wavy-sand/wavy-sand_ao.png", "wavy_ao");
    // // Sand
    // ResourceManager::LoadTexture("assets/textures/pbr/sand/sand_albedo.png", "sand_albedo");
    // ResourceManager::LoadTexture("assets/textures/pbr/sand/sand_normal.png", "sand_normal");
    // ResourceManager::LoadTexture("assets/textures/pbr/sand/sand_metallic.png", "sand_metallic");
    // ResourceManager::LoadTexture("assets/textures/pbr/sand/sand_roughness.png", "sand_roughness");
    // ResourceManager::LoadTexture("assets/textures/pbr/sand/sand_ao.png", "sand_ao");
    // Rounded Alley
    ResourceManager::LoadTexture("assets/textures/pbr/rounded-alley-brick/rounded-alley-brick_albedo.png", "alley_albedo");
    ResourceManager::LoadTexture("assets/textures/pbr/rounded-alley-brick/rounded-alley-brick_normal.png", "alley_normal");
    ResourceManager::LoadTexture("assets/textures/pbr/rounded-alley-brick/rounded-alley-brick_metallic.png", "alley_metallic");
    ResourceManager::LoadTexture("assets/textures/pbr/rounded-alley-brick/rounded-alley-brick_roughness.png", "alley_roughness");
    ResourceManager::LoadTexture("assets/textures/pbr/rounded-alley-brick/rounded-alley-brick_ao.png", "alley_ao");
    // Load HDR Textures
    ResourceManager::LoadHDRTexture(m_SkyboxFilename.c_str(), "skybox_hdr");
    // Load Shaders
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "light_source");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/postProcessingFS.glsl", nullptr, "post_proc");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedFS.glsl", nullptr, "pbr_lighting_textured");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/equirectangularToCubemapFS.glsl", nullptr, "equirectangular_to_cubemap");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/irradianceConvolutionFS.glsl", nullptr, "irradiance");
    ResourceManager::LoadShader("assets/shaders/cubemapVS.glsl", "assets/shaders/prefiltermapFS.glsl", nullptr, "prefilter");
    ResourceManager::LoadShader("assets/shaders/hdrSkyboxVS.glsl", "assets/shaders/hdrSkyboxFS.glsl", nullptr, "hdr_skybox");
    ResourceManager::LoadShader("assets/shaders/brdfVS.glsl", "assets/shaders/brdfFS.glsl", nullptr, "brdf");
    ResourceManager::LoadShader("assets/shaders/blurVS.glsl", "assets/shaders/blurFS.glsl", nullptr, "blur");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/depthQuadFS.glsl", nullptr, "depth_quad");
    ResourceManager::LoadShader("assets/shaders/depthMapVS.glsl", "assets/shaders/depthMapFS.glsl", nullptr, "depth_map");
    ResourceManager::LoadShader("assets/shaders/depthCubeMapVS.glsl", "assets/shaders/depthCubeMapFS.glsl", "assets/shaders/depthCubeMapGS.glsl", "depth_cube_map");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/downsampleFS.glsl", nullptr, "downsample");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/upsampleFS.glsl", nullptr, "upsample");
    ResourceManager::LoadShader("assets/shaders/instancingNormalsVS.glsl", "assets/shaders/instancingNormalsFS.glsl", nullptr, "instancing_normals");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/debugFS.glsl", nullptr, "debug_quad");
    ResourceManager::LoadShader("assets/shaders/gizmoVS.glsl", "assets/shaders/gizmoFS.glsl", nullptr, "trans_gizmo");
    ResourceManager::LoadShader("assets/shaders/gizmoVS.glsl", "assets/shaders/stencilHighlightingFS.glsl", nullptr, "stencil_highlighting");

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
    ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.bloomStrength", m_BloomStrength);

    // PBR Settings
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("material.albedo", m_Albedo);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.metallic", m_Metallic);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.roughness", m_Roughness);
    ResourceManager::GetShader("pbr_lighting").Use().SetFloat("material.ao", m_AO);

    // Object Settings
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("object.isGLTF", 0);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("object.emissiveIntensity", m_EmissiveIntensity);

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
    m_PointLightIntensities.push_back(20.0f);
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

    // Arrow Normals
    ResourceManager::GetShader("instancing_normals").Use().SetFloat(2, m_ArrowNormalsSize);

    multisampleFBO = FrameBuffer();
    multisampleFBO.Bind();
    multisampleFBO.TextureAttachment(2, 0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA, m_Width, m_Height);
    multisampleFBO.RenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    hdrFBO = FrameBuffer();
    hdrFBO.Bind();
    hdrFBO.TextureAttachment(2, 0, GL_TEXTURE_2D, GL_RGBA16F, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    debugFBO = FrameBuffer();
    debugFBO.Bind();
    debugFBO.TextureAttachment(1, 0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

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

    bloomFBO = FrameBuffer();
    bloomFBO.Bind();
    bloomFBO.BloomAttachment(m_Width, m_Height, 6);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    createSkybox();

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
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("far_plane", farPlane);
}

void SandboxLayer::OnUpdate()
{
    glm::mat4 projView = m_Camera.Matrix(m_Camera.m_Fov, static_cast<float>(m_Width) / m_Height, m_Camera.m_NearPlane, m_Camera.m_FarPlane);
    ResourceManager::GetShader("light_source").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("instancing_normals").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("trans_gizmo").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("stencil_highlighting").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(0, projView);
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetVector3f("camPos", m_Camera.m_Position);
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(0, m_Camera.GetProjectionMatrix());
    ResourceManager::GetShader("hdr_skybox").Use().SetMatrix4(1, m_Camera.GetViewMatrix());

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

        glm::mat4 plane_model = glm::mat4(1.0f);
        plane_model = glm::translate(plane_model, glm::vec3(0.0f, -0.01f, 0.0f));
        plane_model = glm::scale(plane_model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, plane_model);
        renderPlane(GL_TRIANGLES);

        if (m_UsePointLights) {
            for (size_t i = 0; i < m_PointLightPositions.size(); i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, m_PointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
                ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
                renderCube(GL_TRIANGLES);
            }
        }

        glm::mat4 object_model = glm::mat4(1.0f);
        object_model = glm::translate(object_model, m_ObjectPosition);
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        object_model = glm::scale(object_model, glm::vec3(1.0f) * m_ObjectScale);
        renderObject(GL_TRIANGLES, ResourceManager::GetShader("depth_map"), ResourceManager::GetModel("helmet"), object_model);

        for (GLuint i = 0; i < 5; i++) {
            for (GLuint j = 0; j < 5; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-6.0f + (i*3), 0.5f + (j*3), 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
                renderSphere(GL_TRIANGLE_STRIP);
            }
        }

        for (GLuint i = 0; i < 5; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-6.0f + (i*3), 6.0f, 4.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            ResourceManager::GetShader("depth_map").Use().SetMatrix4(1, model);
            renderSphere(GL_TRIANGLE_STRIP);
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

        glm::mat4 plane_model = glm::mat4(1.0f);
        plane_model = glm::translate(plane_model, glm::vec3(0.0f, -0.01f, 0.0f));
        plane_model = glm::scale(plane_model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(1, plane_model);
        renderPlane(GL_TRIANGLES);

        glm::mat4 object_model = glm::mat4(1.0f);
        object_model = glm::translate(object_model, m_ObjectPosition);
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        object_model = glm::rotate(object_model, glm::radians(m_RotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        object_model = glm::scale(object_model, glm::vec3(1.0f) * m_ObjectScale);
        renderObject(GL_TRIANGLES, ResourceManager::GetShader("depth_cube_map"), ResourceManager::GetModel("helmet"), object_model);

        for (GLuint i = 0; i < 5; i++) {
            for (GLuint j = 0; j < 5; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-6.0f + (i*3), 0.5f + (j*3), 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(1, model);
                renderSphere(GL_TRIANGLE_STRIP);
            }
        }

        for (GLuint i = 0; i < 5; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-6.0f + (i*3), 6.0f, 4.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            ResourceManager::GetShader("depth_cube_map").Use().SetMatrix4(1, model);
            renderSphere(GL_TRIANGLE_STRIP);
        }

        FrameBuffer::UnBind();

        glViewport(0, 0, m_Width, m_Height);
    }

    // Render Normal Scene
    multisampleFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (m_UsePolygonLines)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render Plane
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    ResourceManager::GetTexture("alley_albedo").Bind(3);
    ResourceManager::GetTexture("alley_normal").Bind(4);
    ResourceManager::GetTexture("alley_metallic").Bind(5);
    ResourceManager::GetTexture("alley_roughness").Bind(6);
    ResourceManager::GetTexture("alley_ao").Bind(7);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE9);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE10);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    glm::mat4 plane_model = glm::mat4(1.0f);
    plane_model = glm::translate(plane_model, glm::vec3(0.0f, 0.0f, 0.0f));
    plane_model = glm::scale(plane_model, glm::vec3(1.0f, 1.0f, 1.0f));
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, plane_model);
    renderPlane(GL_TRIANGLES);
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
            renderCube(GL_TRIANGLES);
        }
    }

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
            renderSphere(GL_TRIANGLE_STRIP);
        }
    }
    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Textured Spheres
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE9);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE10);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    {
        ResourceManager::GetTexture("wall_albedo").Bind(3);
        ResourceManager::GetTexture("wall_normal").Bind(4);
        ResourceManager::GetTexture("wall_metallic").Bind(5);
        ResourceManager::GetTexture("wall_roughness").Bind(6);
        ResourceManager::GetTexture("wall_ao").Bind(7);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f, 6.0f, 4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
        renderSphere(GL_TRIANGLE_STRIP);

        ResourceManager::GetTexture("plastic_albedo").Bind(3);
        ResourceManager::GetTexture("plastic_normal").Bind(4);
        ResourceManager::GetTexture("plastic_metallic").Bind(5);
        ResourceManager::GetTexture("plastic_roughness").Bind(6);
        ResourceManager::GetTexture("plastic_ao").Bind(7);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f + 3, 6.0f, 4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
        renderSphere(GL_TRIANGLE_STRIP);

        ResourceManager::GetTexture("grass_albedo").Bind(3);
        ResourceManager::GetTexture("grass_normal").Bind(4);
        ResourceManager::GetTexture("grass_metallic").Bind(5);
        ResourceManager::GetTexture("grass_roughness").Bind(6);
        ResourceManager::GetTexture("grass_ao").Bind(7);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f + 6, 6.0f, 4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
        renderSphere(GL_TRIANGLE_STRIP);

        ResourceManager::GetTexture("gold_albedo").Bind(3);
        ResourceManager::GetTexture("gold_normal").Bind(4);
        ResourceManager::GetTexture("gold_metallic").Bind(5);
        ResourceManager::GetTexture("gold_roughness").Bind(6);
        ResourceManager::GetTexture("gold_ao").Bind(7);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f + 9, 6.0f, 4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
        renderSphere(GL_TRIANGLE_STRIP);

        ResourceManager::GetTexture("rusted_albedo").Bind(3);
        ResourceManager::GetTexture("rusted_normal").Bind(4);
        ResourceManager::GetTexture("rusted_metallic").Bind(5);
        ResourceManager::GetTexture("rusted_roughness").Bind(6);
        ResourceManager::GetTexture("rusted_ao").Bind(7);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f + 12, 6.0f, 4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ResourceManager::GetShader("pbr_lighting_textured").Use().SetMatrix4(1, model);
        renderSphere(GL_TRIANGLE_STRIP);
    }

    Texture2D::UnBind();
    Texture2D::UnBindCubemap();

    // Render Models
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("object.isGLTF", 1);
    m_Irradiancemap.BindCubemap(0);
    m_Prefiltermap.BindCubemap(1);
    m_BRDFLUTTexture.Bind(2);
    if (m_UseDirShadows && m_UseDirLight) {
        glActiveTexture(GL_TEXTURE9);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
    }
    if (m_UsePointShadows && m_UsePointLights) {
        glActiveTexture(GL_TEXTURE10);
        depthCubeMapFBO.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    glm::mat4 object_model = glm::mat4(1.0f);
    object_model = glm::translate(object_model, m_ObjectPosition);
    object_model = glm::rotate(object_model, glm::radians(m_RotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
    object_model = glm::rotate(object_model, glm::radians(m_RotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
    object_model = glm::rotate(object_model, glm::radians(m_RotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
    object_model = glm::scale(object_model, glm::vec3(1.0f) * m_ObjectScale);

    if (m_UseObjectHighlighting) {
        highlightRenderObject(GL_TRIANGLES, ResourceManager::GetShader("pbr_lighting_textured"), ResourceManager::GetShader("stencil_highlighting"), ResourceManager::GetModel("helmet"), object_model);
        if (m_UseTransGizmo) {
            glDepthFunc(GL_ALWAYS);
            renderTranslationGizmo(GL_TRIANGLES, ResourceManager::GetShader("trans_gizmo"), m_ObjectPosition);
            glDepthFunc(GL_LEQUAL);
        }
    }
    else {
        renderObject(GL_TRIANGLES, ResourceManager::GetShader("pbr_lighting_textured"), ResourceManager::GetModel("helmet"), object_model);
    }

    if (m_UseArrowNormals) {
        renderNormalsInstanced(ResourceManager::GetShader("instancing_normals"), ResourceManager::GetModel("helmet").instancedArrowsVBO, object_model, ResourceManager::GetModel("helmet").instancedArrowsSize);
    }

    Texture2D::UnBind();
    Texture2D::UnBindCubemap();
    ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("object.isGLTF", 0);

    // Render Skybox
    m_EnvCubemap.BindCubemap(0);
    // m_Irradiancemap.BindCubemap(0);
    // m_Prefiltermap.BindCubemap(0);
    ResourceManager::GetShader("hdr_skybox").Use();
    renderCube(GL_TRIANGLES);
    Texture2D::UnBindCubemap();

    FrameBuffer::UnBind();

    // Reset polygon mode to fill no matter what
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    multisampleFBO.Blit(hdrFBO, m_Width, m_Height);
    FrameBuffer::CheckStatus();
    FrameBuffer::UnBind();

    // Bloom
    if (m_UseBloom) {
        renderBloomTexture();
    }

    // Generate Image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    imguiFBO.Bind();
    glActiveTexture(GL_TEXTURE0);
    hdrFBO.BindTexture(GL_TEXTURE_2D, 0);
    if (m_UseBloom) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomFBO.GetMipChain()[0].texture);
    }
    ResourceManager::GetShader("post_proc").Use();
    renderQuad(GL_TRIANGLES);
    Texture2D::UnBind();
    FrameBuffer::UnBind();

    // Debug Image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    debugFBO.Bind();
    if (m_DebugDepthMap) {
        glActiveTexture(GL_TEXTURE0);
        depthMapFBO.BindTexture(GL_TEXTURE_2D, 0);
        ResourceManager::GetShader("depth_quad").Use();
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        hdrFBO.BindTexture(GL_TEXTURE_2D, 0);
        ResourceManager::GetShader("debug_quad").Use();
    }
    renderQuad(GL_TRIANGLES);
    Texture2D::UnBind();
    FrameBuffer::UnBind();
    glEnable(GL_DEPTH_TEST);
}

void SandboxLayer::renderBloomTexture()
{
    bloomFBO.Bind();

    // Render Downsamples
    ResourceManager::GetShader("downsample").Use().SetVector2f("srcResolution", m_SrcResolution);

    if (m_KarisAverageOnDownsample) {
        ResourceManager::GetShader("downsample").Use().SetInteger("mipLevel", 0);
    }

    glActiveTexture(GL_TEXTURE0);
    hdrFBO.BindTexture(GL_TEXTURE_2D, 1);

    // Progressively downsample through the mip chain
    for (size_t i = 0; i < bloomFBO.GetMipChain().size(); i++)
    {
        const BloomMip& mip = bloomFBO.GetMipChain()[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        renderQuad(GL_TRIANGLES);

        // Set current mip resolution as srcResolution for next iteration
        ResourceManager::GetShader("downsample").Use().SetVector2f("srcResolution", mip.size);
        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // Disable Karis average for consequent downsamples
        if (i == 0) {
            ResourceManager::GetShader("downsample").Use().SetInteger("mipLevel", 1);
        }
    }
    Texture2D::UnBind();

    // Render Upsamples
    ResourceManager::GetShader("upsample").Use().SetFloat("filterRadius", m_BloomFilterRadius);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (size_t i = bloomFBO.GetMipChain().size() - 1; i > 0; i--)
    {
        const BloomMip& mip = bloomFBO.GetMipChain()[i];
        const BloomMip& nextMip = bloomFBO.GetMipChain()[i-1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        renderQuad(GL_TRIANGLES);
    }

    // Disable additive blending
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    Texture2D::UnBind();


    FrameBuffer::UnBind();
    glViewport(0, 0, m_Width, m_Height);
}

void SandboxLayer::renderPlane(GLenum mode)
{
    planeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    planeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    planeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(mode, 0, 6);
    planeVBO.UnlinkAttrib(0);
    planeVBO.UnlinkAttrib(1);
    planeVBO.UnlinkAttrib(2);
}

void SandboxLayer::renderCube(GLenum mode)
{
    cubeVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    cubeVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    cubeVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawArrays(mode, 0, 36);
    cubeVBO.UnlinkAttrib(0);
    cubeVBO.UnlinkAttrib(1);
    cubeVBO.UnlinkAttrib(2);
}

void SandboxLayer::renderSphere(GLenum mode)
{
    sphereEBO.Bind();
    sphereVBO.LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    sphereVBO.LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    sphereVBO.LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glDrawElements(mode, indexCount, GL_UNSIGNED_INT, 0);
    sphereVBO.UnlinkAttrib(0);
    sphereVBO.UnlinkAttrib(1);
    sphereVBO.UnlinkAttrib(2);
    sphereEBO.UnBind();
}

void SandboxLayer::renderQuad(GLenum mode)
{
    screenQuadVBO.LinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
    screenQuadVBO.LinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glDrawArrays(mode, 0, 6);
    screenQuadVBO.UnlinkAttrib(0);
    screenQuadVBO.UnlinkAttrib(1);
}

void SandboxLayer::renderObject(GLenum mode, Shader shader, Model model_3d, glm::mat4 model)
{
    shader.Use().SetMatrix4(1, model);
    model_3d.Draw(mode, shader);
}

void SandboxLayer::highlightRenderObject(GLenum mode, Shader shader, Shader highlight_shader, Model model_3d, glm::mat4 model)
{
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    renderObject(mode, shader, model_3d, model);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    model = glm::scale(model, glm::vec3(1.0f) * 1.05f);
    renderObject(mode, highlight_shader, model_3d, model);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glDisable(GL_STENCIL_TEST);
}

void SandboxLayer::renderTranslationGizmo(GLenum mode, Shader shader, glm::vec3 pos)
{
    glm::mat4 cube_model = glm::mat4(1.0f);
    cube_model = glm::translate(cube_model, pos);
    cube_model = glm::scale(cube_model, glm::vec3(m_GizmoSize));
    shader.Use().SetInteger("axis", 3);
    shader.Use().SetMatrix4(1, cube_model);
    renderCube(GL_TRIANGLES);

    glm::mat4 green_model = glm::mat4(1.0f);
    green_model = glm::translate(green_model, pos + glm::vec3(0.0f, m_GizmoSize, 0.0f));
    green_model = glm::scale(green_model, glm::vec3(m_GizmoSize));
    shader.Use().SetInteger("axis", 1);
    renderObject(mode, shader, ResourceManager::GetModel("gizmo_arrow"), green_model);

    glm::mat4 red_model = glm::mat4(1.0f);
    red_model = glm::translate(red_model, pos + glm::vec3(m_GizmoSize, 0.0f, 0.0f));
    red_model = glm::rotate(red_model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    red_model = glm::scale(red_model, glm::vec3(m_GizmoSize));
    shader.Use().SetInteger("axis", 0);
    renderObject(mode, shader, ResourceManager::GetModel("gizmo_arrow"), red_model);

    glm::mat4 blue_model = glm::mat4(1.0f);
    blue_model = glm::translate(blue_model, pos + glm::vec3(0.0f, 0.0f, m_GizmoSize));
    blue_model = glm::rotate(blue_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    blue_model = glm::scale(blue_model, glm::vec3(m_GizmoSize));
    shader.Use().SetInteger("axis", 2);
    renderObject(mode, shader, ResourceManager::GetModel("gizmo_arrow"), blue_model);
}

void SandboxLayer::renderNormalsInstanced(Shader shader, const VertexBuffer &VBO, glm::mat4 model, size_t matrices_size)
{
    shader.Use().SetMatrix4(1, model);
    VBO.LinkAttrib(5, 4, GL_FLOAT, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1);
    for (size_t i = 0; i < ResourceManager::GetModel("gizmo_arrow").meshes.size(); i++) {
        ResourceManager::GetModel("gizmo_arrow").meshes[i].Bind();
        glDrawElementsInstanced(GL_TRIANGLES, ResourceManager::GetModel("gizmo_arrow").meshes[i].indices.size(), GL_UNSIGNED_INT, 0, matrices_size);
        ResourceManager::GetModel("gizmo_arrow").meshes[i].UnBind();
    }
    VBO.UnlinkAttrib(5);
    glVertexAttribDivisor(5, 0);
}

void SandboxLayer::OnImGuiRender()
{
    Application& app = Application::Get();

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Debug Image");

    ImGui::ImageButton((void*)(intptr_t)debugFBO.GetTextureAttachments().at(0), ImVec2(m_Width, m_Height), ImVec2(0, 1), ImVec2(1, 0), 0);
    ImGui::PopStyleVar();

    ImGui::End();

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

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::Begin("Settings");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Camera", base_flags)) {
        ImGui::DragFloat3("Position", (float*)&m_Camera.m_Position, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::DragFloat3("Orientation", (float*)&m_Camera.m_Orientation, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
        ImGui::SliderFloat("Field of view", &m_Camera.m_Fov, 1.0f, 90.0f, "%.2f");
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Light", base_flags)) {
        if (ImGui::TreeNodeEx("Directional Light", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseDirLight)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("dirLight.use", m_UseDirLight);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("dirLight.use", m_UseDirLight);
            }
            if (m_UseDirLight) {
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
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Point Lights", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UsePointLights)) {
                ResourceManager::GetShader("pbr_lighting").Use().SetInteger("pointLight.use", m_UsePointLights);
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("pointLight.use", m_UsePointLights);
            }
            if (m_UsePointLights) {
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
            }
            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("PBR Materials", base_flags)) {
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

    ImGui::Separator();
    if (ImGui::CollapsingHeader("3D Object", base_flags)) {
        if (ImGui::TreeNodeEx("Edit Object")) {
            m_UseObjectHighlighting = true;
            ImGui::Checkbox("Display Translation Gizmo", &m_UseTransGizmo);
            if (m_UseTransGizmo) {
                ImGui::SliderFloat("Gizmo Size", (float*)&m_GizmoSize, 0.01f, 1.0f, "%.2f");
            }
            ImGui::DragFloat3("Position", (float*)&m_ObjectPosition, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
            if (ImGui::DragFloat3("Rotation", (float*)&m_RotationAngles, 0.01f, -180.0f, 180.0f, "%.2f")) {
                m_RotationAngleX = m_RotationAngles.x;
                m_RotationAngleY = m_RotationAngles.y;
                m_RotationAngleZ = m_RotationAngles.z;
            }
            ImGui::DragFloat("Scale", &m_ObjectScale, 0.01f, 0.0f, FLT_MAX, "%.2f");
            if (ImGui::DragFloat("Emissive", &m_EmissiveIntensity, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                ResourceManager::GetShader("pbr_lighting_textured").Use().SetFloat("object.emissiveIntensity", m_EmissiveIntensity);
            }
            ImGui::Checkbox("Display Split Normals", &m_UseArrowNormals);
            if (m_UseArrowNormals && ImGui::SliderFloat("Normals Arrow Size", (float*)&m_ArrowNormalsSize, 0.001f, 0.1f, "%.3f")) {
                ResourceManager::GetShader("instancing_normals").Use().SetFloat(2, m_ArrowNormalsSize);
            }
            ImGui::TreePop();
        }
        else {
            m_UseObjectHighlighting = false;
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Skybox", base_flags)) {
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

                ResourceManager::LoadHDRTexture(m_SkyboxFilename.c_str(), "skybox_hdr");
                m_EnvCubemap.Destroy();
                m_Irradiancemap.Destroy();
                m_Prefiltermap.Destroy();
                m_BRDFLUTTexture.Destroy();
                captureFBO.Bind();
                captureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_EnvCubemapWidth, m_EnvCubemapHeight);
                FrameBuffer::CheckStatus();
                FrameBuffer::UnBind();
                createSkybox();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Text("%s", m_SkyboxFilename.c_str());
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Debug", base_flags)) {

        if (ImGui::TreeNodeEx("Debug Image Window", base_flags)) {
            ImGui::Checkbox("Depth Map (Directional Light)", &m_DebugDepthMap);
            ImGui::TreePop();
        }

        ImGui::Checkbox("Polygon Line Mode", &m_UsePolygonLines);
        if (ImGui::Checkbox("Depth Cube Map (Point Light)", &m_DebugDepthCubeMap)) {
            m_UseDebugNormals = false;
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);

            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugNormals", m_UseDebugNormals);
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugNormals", m_UseDebugNormals);
            ResourceManager::GetShader("light_source").Use().SetInteger("debugNormals", m_UseDebugNormals);
        }
        if (ImGui::Checkbox("Normal Vectors", &m_UseDebugNormals)) {
            m_DebugDepthCubeMap = false;
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugNormals", m_UseDebugNormals);
            ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugNormals", m_UseDebugNormals);
            ResourceManager::GetShader("light_source").Use().SetInteger("debugNormals", m_UseDebugNormals);

            ResourceManager::GetShader("pbr_lighting").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
            ResourceManager::GetShader("pbr_lighting_textured").Use().SetInteger("debugDepthCubeMap", m_DebugDepthCubeMap);
        }
    }

    ImGui::Separator();
    ImGui::End();

    ImGui::Begin("Post Processing");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Kernel Effects", base_flags)) {
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
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("General Effects", base_flags)) {
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
        if (ImGui::TreeNodeEx("Bloom", base_flags)) {
            ImGui::SameLine();
            if (ImGuiLayer::ToggleButton(" ", &m_UseBloom)) {
                ResourceManager::GetShader("post_proc").Use().SetInteger("postProcessing.bloom", m_UseBloom);
            }
            if (m_UseBloom) {
                if (ImGui::DragFloat("Bloom Intensity", &m_BloomStrength, 0.01f, 0.0f, 0.5f, "%.2f")) {
                    ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.bloomStrength", m_BloomStrength);
                }
                if (ImGui::SliderFloat("Filter Radius", &m_BloomFilterRadius, 0.0f, 0.05f, "%.3f")) {
                    ResourceManager::GetShader("upsample").Use().SetFloat("filterRadius", m_BloomFilterRadius);
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("HDR Settings", base_flags)) {
        if (ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, FLT_MAX, "%.2f")) {
            ResourceManager::GetShader("post_proc").Use().SetFloat("postProcessing.exposure", m_Exposure);
        }
    }

    ImGui::Separator();
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
        multisampleFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA, m_Width, m_Height);
        multisampleFBO.ResizeRenderBufferAttachment(GL_TRUE, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        hdrFBO.Bind();
        hdrFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA16F, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        imguiFBO.Bind();
        imguiFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        debugFBO.Bind();
        debugFBO.ResizeTextureAttachment(0, GL_TEXTURE_2D, GL_RGBA8, m_Width, m_Height);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        bloomFBO.Bind();
        bloomFBO.ResizeBloomAttachment(m_Width, m_Height, 6);
        FrameBuffer::CheckStatus();
        FrameBuffer::UnBind();

        GL_INFO("Resizing Generated Image to : {0} {1}", m_Width, m_Height);

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
    imguiFBO.Destroy();
    debugFBO.Destroy();
    captureFBO.Destroy();
    depthMapFBO.Destroy();
    depthCubeMapFBO.Destroy();
    bloomFBO.Destroy();

    m_EnvCubemap.Destroy();
    m_Irradiancemap.Destroy();
    m_Prefiltermap.Destroy();
    m_BRDFLUTTexture.Destroy();
}

void SandboxLayer::createSkybox()
{
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
    ResourceManager::GetTexture("skybox_hdr").Bind(0);
    ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(0, captureProjection);
    glViewport(0, 0, m_EnvCubemapWidth, m_EnvCubemapHeight);
    captureFBO.Bind();
    for (GLuint i = 0; i < 6; i++) {
        ResourceManager::GetShader("equirectangular_to_cubemap").Use().SetMatrix4(1, captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap.ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube(GL_TRIANGLES);
    }
    FrameBuffer::UnBind();
    Texture2D::UnBind();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap.ID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    Texture2D::UnBindCubemap();

    // create an irradiance cubemap, and re-scale captureFBO to irradiance scale
    m_Irradiancemap.Internal_Format = GL_RGB16F;
    m_Irradiancemap.Image_Format = GL_RGB;
    m_Irradiancemap.Data_Type = GL_FLOAT;
    m_Irradiancemap.Wrap_S = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_T = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Wrap_R = GL_CLAMP_TO_EDGE;
    m_Irradiancemap.Filter_Min = GL_LINEAR;
    m_Irradiancemap.Filter_Max = GL_LINEAR;
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
        renderCube(GL_TRIANGLES);
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
    captureFBO.Bind();
    GLuint maxMipLevels = 5;
    for (GLuint mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
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
            renderCube(GL_TRIANGLES);
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
    captureFBO.Bind();
    captureFBO.ResizeRenderBuffer(GL_DEPTH24_STENCIL8, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLUTTexture.ID, 0);

    glViewport(0, 0, m_BRDFLUTTextureWidth, m_BRDFLUTTextureHeight);
    ResourceManager::GetShader("brdf").Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad(GL_TRIANGLES);
    FrameBuffer::UnBind();

    glViewport(0, 0, m_Width, m_Height);
}
