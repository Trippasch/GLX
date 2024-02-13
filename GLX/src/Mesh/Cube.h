#pragma once

#include "Mesh/Entity.h"

class Cube : public Entity
{
public:
    VertexBuffer* pVBO = nullptr;

    Cube() : pVBO(nullptr)
    {}

    Cube(VertexBuffer& vbo, float* vertices, size_t size) : pVBO(&vbo)
    {
        boundingVolume = std::make_shared<AABB>(generateAABB(vertices, size));
        boundingVolume.get()->drawAABBSetup();
    }

    AABB generateAABB(const float* vertices, size_t size)
    {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

        // Each vertex consists of 8 values (3 for position, 3 for normals, 2 for texcoords)
        // So we step 8 values at a time
        for (int i = 0; i < size; i += 8) {
            glm::vec3 vertexPosition = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);

            minAABB.x = std::min(minAABB.x, vertexPosition.x);
            minAABB.y = std::min(minAABB.y, vertexPosition.y);
            minAABB.z = std::min(minAABB.z, vertexPosition.z);

            maxAABB.x = std::max(maxAABB.x, vertexPosition.x);
            maxAABB.y = std::max(maxAABB.y, vertexPosition.y);
            maxAABB.z = std::max(maxAABB.z, vertexPosition.z);
        }
        return AABB(minAABB, maxAABB);
    }

    void drawSelfAndChildSimple(GLenum& mode, Shader& shader) override
    {
        if (!children.empty()) {
            for (auto&& child : children) {
                child->drawSelfAndChildSimple(mode, shader);
            }
        }
        else {
            shader.Use().SetMatrix4(1, transform.getModelMatrix());
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawArrays(mode, 0, 36);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);
        }
    }

    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, Shader& shader, unsigned int& display, unsigned int& total) override
    {
        if (!children.empty()) {
            total++;
            for (auto&& child : children) {
                child->drawSelfAndChild(mode, frustum, shader, display, total);
            }
        }
        else if (boundingVolume->isOnFrustum(frustum, transform)) {
            shader.Use().SetVector3f("material.albedo", material.getAlbedo());
            shader.Use().SetFloat("material.metallic", material.getMetallic());
            shader.Use().SetFloat("material.roughness", material.getRoughness());
            shader.Use().SetFloat("material.ao", material.getAO());
            shader.Use().SetMatrix4(1, transform.getModelMatrix());
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawArrays(mode, 0, 36);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);
            boundingVolume.get()->drawAABB(transform.getModelMatrix());

            display++;
        }
    }
};