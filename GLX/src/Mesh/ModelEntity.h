#pragma once

#include "Mesh/Entity.h"
#include "Resources/Animator.h"
#include "Renderer/RendererLayer.h"

class RendererLayer;

class ModelEntity : public Entity
{
private:
    Model* pModel = nullptr;
    RendererLayer* m_Renderer;
    Animator* m_Animator;
    bool m_HasAnimation = false;
    bool m_Animate = false;
    int m_CurrentAnimation = 0;
    const char* m_Animations[3] = { "T-Pose", "Idle", "Run" };

public:
    ModelEntity() : pModel(nullptr), m_Renderer(nullptr)
    {}
    ModelEntity(Model& model, RendererLayer* renderer, bool hasAnimation = false);

    void Destroy();

    void drawSelfAndChildSimple(GLenum& mode, Shader& shader) override;
    void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void renderGUI(int i) override;

private:
    AABB generateAABB(const Model& model);
    void updateAABB(const Model& model, AABB& aabb);
    SphereBV generateSphereBV(const Model& model);
    std::vector<glm::vec3> transformVertices(const Model& model, const std::vector<glm::mat4>& boneMatrices);
};