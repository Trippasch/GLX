#pragma once

#include "Mesh/Entity.h"
#include "Renderer/RendererLayer.h"

class RendererLayer;

class Plane : public Entity
{
private:
    VertexBuffer* pVBO = nullptr;
    RendererLayer* m_Renderer;

public:
    Plane() : pVBO(nullptr), m_Renderer(nullptr)
    {}
    Plane(VertexBuffer& vbo, float* vertices, size_t size, RendererLayer* renderer);

    void drawSelfAndChildSimple(GLenum& mode, Shader& shader) override;
    void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void renderGUI(int i) override;

private:
    AABB generateAABB(const float* vertices, size_t size);
    SphereBV generateSphereBV(const float* vertices, size_t size);
};