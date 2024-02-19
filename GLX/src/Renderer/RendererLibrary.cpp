#include "Renderer/RendererLibrary.h"

RendererLibrary::RendererLibrary()
{
    CreateCubeVBO();
    CreateQuadVBO();
    CreatePlaneVBO();
    CreateSphereData();

    // Load default textures
    ResourceManager::LoadTexture("assets/textures/default/albedo.png", "default_albedo");
    ResourceManager::LoadTexture("assets/textures/default/normal.png", "default_normal");
    ResourceManager::LoadTexture("assets/textures/default/metallic.png", "default_metallic");
    ResourceManager::LoadTexture("assets/textures/default/roughness.png", "default_roughness");
    ResourceManager::LoadTexture("assets/textures/default/ao.png", "default_ao");
    ResourceManager::LoadTexture("assets/textures/default/emissive.png", "default_emissive");

    // Load gizmo shader
    ResourceManager::LoadShader("assets/shaders/gizmoVS.glsl", "assets/shaders/gizmoFS.glsl", nullptr, "gizmo");
    ResourceManager::GetShader("gizmo").Use().SetVector3f("color", glm::vec3(1.0f, 1.0f, 1.0f));

    // Load light source shader
    ResourceManager::LoadShader("assets/shaders/lightSourceVS.glsl", "assets/shaders/lightSourceFS.glsl", nullptr, "lightSource");
    ResourceManager::GetShader("lightSource").Use().SetVector3f("color", glm::vec3(0.0f, 0.0f, 0.0f));

    // Load depth cube map and debug depth cube map shaders
    ResourceManager::LoadShader("assets/shaders/depthCubeMapVS.glsl", "assets/shaders/depthCubeMapFS.glsl", "assets/shaders/depthCubeMapGS.glsl", "depth_cube_map");
    ResourceManager::LoadShader("assets/shaders/debugDepthCubeMapVS.glsl", "assets/shaders/debugDepthCubeMapFS.glsl", nullptr, "debug_depth_cube_map");

    // Load depth map and debug depth map shaders
    ResourceManager::LoadShader("assets/shaders/depthMapVS.glsl", "assets/shaders/depthMapFS.glsl", "assets/shaders/depthMapGS.glsl", "depth_map");
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/debugDepthMapFS.glsl", nullptr, "debug_depth_map");

    // Load Debug Shader
    ResourceManager::LoadShader("assets/shaders/quadVS.glsl", "assets/shaders/debugFS.glsl", nullptr, "debug_quad");

    // Load Model
    ResourceManager::LoadModel("assets/objects/helmet/DamagedHelmet.gltf", "helmet");

    // Lighting
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedFS.glsl", nullptr, "pbr_lighting_textured");
    ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrTexturedGLTFFS.glsl", nullptr, "pbr_lighting_textured_gltf");
}

RendererLibrary::~RendererLibrary()
{
    m_CubeVBO.Destroy();
    m_QuadVBO.Destroy();
    m_PlaneVBO.Destroy();
    m_SphereVBO.Destroy();
    m_SphereEBO.Destroy();
}

void RendererLibrary::CreateCubeVBO()
{
    m_CubeVBO = VertexBuffer(&m_CubeVertices, sizeof(m_CubeVertices), GL_STATIC_DRAW);
}

void RendererLibrary::CreateQuadVBO()
{
    m_QuadVBO = VertexBuffer(&m_QuadVertices, sizeof(m_QuadVertices), GL_STATIC_DRAW);
}

void RendererLibrary::CreatePlaneVBO()
{
    m_PlaneVBO = VertexBuffer(&m_PlaneVertices, sizeof(m_PlaneVertices), GL_STATIC_DRAW);
}

void RendererLibrary::CreateSphereData()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;

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
                m_SphereIndices.push_back(y * (X_SEGMENTS + 1) + x);
                m_SphereIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                m_SphereIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                m_SphereIndices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    m_SphereIndicesCount = static_cast<GLuint>(m_SphereIndices.size());

    for (size_t i = 0; i < positions.size(); ++i)
    {
        m_SphereVertices.push_back(positions[i].x);
        m_SphereVertices.push_back(positions[i].y);
        m_SphereVertices.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            m_SphereVertices.push_back(normals[i].x);
            m_SphereVertices.push_back(normals[i].y);
            m_SphereVertices.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            m_SphereVertices.push_back(uv[i].x);
            m_SphereVertices.push_back(uv[i].y);
        }
    }
    m_SphereVBO = VertexBuffer(&m_SphereVertices[0], m_SphereVertices.size() * sizeof(float), GL_STATIC_DRAW);
    m_SphereEBO = IndexBuffer(&m_SphereIndices[0], m_SphereIndices.size() * sizeof(GLuint), GL_STATIC_DRAW);
}