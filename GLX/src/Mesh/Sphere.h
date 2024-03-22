#pragma once

#include "Mesh/Entity.h"
#include "Renderer/RendererLayer.h"

class RendererLayer;

class Sphere : public Entity
{
private:
    VertexBuffer* pVBO = nullptr;
    IndexBuffer* pEBO = nullptr;
    GLuint pIndexCount;
    RendererLayer* m_Renderer;

public:
    Sphere() : pVBO(nullptr), pEBO(nullptr), pIndexCount(0), m_Renderer(nullptr)
    {}
    Sphere(VertexBuffer& vbo, float* vertices, size_t size, IndexBuffer& ebo, GLuint indexCount, RendererLayer* renderer);

    void drawSelfAndChildSimple(GLenum& mode, Shader& shader) override;
    void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override;
    void renderGUI(int i) override;

private:
    AABB generateAABB(const float* vertices, size_t size);
    SphereBV generateSphereBV(const float* vertices, size_t size);
};