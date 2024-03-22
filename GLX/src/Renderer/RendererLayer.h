#pragma once

#include "Core/Layer.h"
#include "Core/Log.h"

#include "Camera/Camera.h"

#include "Resources/FrameBuffer.h"
#include "Resources/UniformBuffer.h"

#include "Mesh/ModelEntity.h"
#include "Mesh/Cube.h"
#include "Mesh/Sphere.h"
#include "Mesh/Plane.h"

#include "Renderer/RendererLibrary.h"
#include "Renderer/PBR.h"
#include "Renderer/PostProcessor.h"

#include "ImGui/ImGuiLayer.h"
#include "ImGui/ImGuiFileDialog.h"

#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class PBR;
class DirectionalLight;
class PointLight;
class PostProcessor;
class ModelEntity;
class Plane;
class Cube;
class Sphere;

constexpr ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen;

class RendererLayer : public Layer
{
public:
    RendererLayer();
    ~RendererLayer() = default;

    virtual void OnAttach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;
    virtual void OnDetach() override;

    void SetWidth(GLuint width) { this->m_Width = width; }
    const GLuint& GetWidth() const { return m_Width; }

    void SetHeight(GLuint height) { this->m_Height = height; }
    const GLuint& GetHeight() const { return m_Height; }

    Camera& GetCamera() { return m_Camera; }

    void RenderCube(GLenum mode);
    void RenderQuad(GLenum mode);

    void AddPlane();
    void AddCube();
    void AddSphere();
    void AddModel();

    void AddLight(DirectionalLight* light);
    void AddLight(PointLight* light);
    void RemoveLight(DirectionalLight* light);
    void RemoveLight(PointLight* light);

    const UniformBuffer& GetMatricesUBO() const { return m_MatricesUBO; }
    const UniformBuffer& GetObjectUBO() const { return m_ObjectUBO; }

    const float& GetDeltaTime() const { return m_DeltaTime; }

private:
    GLFWwindow* m_Window;
    GLuint m_Width = 800;
    GLuint m_Height = 600;
    bool m_VSync = false;

    float m_DeltaTime = 0.0f;
    float m_LastFrame = 0.0f;
    Camera m_Camera;
    Frustum m_CamFrustum;

    // Library
    RendererLibrary* m_RendererLibrary;

    // PBR
    PBR* m_PBR;

    // Lights
    std::vector<DirectionalLight*> m_DirectionalLights;
    std::vector<PointLight*> m_PointLights;

    // PostProcessor
    PostProcessor* m_PostProcessor;

    // FrameBuffers
    FrameBuffer m_MultisampleFBO;
    FrameBuffer m_ImGUIFBO;
    FrameBuffer m_DebugFBO;

    // UniformBuffers
    UniformBuffer m_MatricesUBO;
    UniformBuffer m_LightsUBO;
    UniformBuffer m_CameraUBO;
    UniformBuffer m_ObjectUBO;

    // Mesh Entities
    Plane* m_Planes;
    Cube* m_Cubes;
    Sphere* m_Spheres;
    ModelEntity* m_Models;
    int m_ObjectsID = 0;
    std::string m_ModelFilePath = "None";
    bool m_IsGLTF = true;
    bool m_IsAnimated = false;

    // Skybox Properties
    bool m_UseSkybox = true;
    glm::vec3 m_BackgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

    // Debug Properties
    bool m_UseDebugWindow = false;
    bool m_UsePolygonLines = false;
    bool m_DebugDepthCubeMap = false;
    bool m_DebugDepthMap = false;
    GLint m_DebugDepthMapIndex = 0;

    bool imGuiResize();
};