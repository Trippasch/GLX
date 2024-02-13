#pragma once

#include "Mesh/Entity.h"

class ModelEntity : public Entity
{
public:
    Model* pModel = nullptr;

    ModelEntity() : pModel(nullptr)
    {}

    ModelEntity(Model& model) : pModel(&model)
    {
        boundingVolume = std::make_shared<AABB>(generateAABB(model));
        boundingVolume.get()->drawAABBSetup();
    }

    AABB generateAABB(const Model& model)
    {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& mesh : model.meshes) {
            for (auto&& vertex : mesh.vertices) {
                minAABB.x = std::min(minAABB.x, vertex.Position.x);
                minAABB.y = std::min(minAABB.y, vertex.Position.y);
                minAABB.z = std::min(minAABB.z, vertex.Position.z);

                maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
                maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
                maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
            }
        }
        return AABB(minAABB, maxAABB);
    }

    SphereBV generateSphereBV(const Model& model)
    {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& mesh : model.meshes) {
            for (auto&& vertex : mesh.vertices) {
                minAABB.x = std::min(minAABB.x, vertex.Position.x);
                minAABB.y = std::min(minAABB.y, vertex.Position.y);
                minAABB.z = std::min(minAABB.z, vertex.Position.z);

                maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
                maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
                maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
            }
        }

        return SphereBV((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
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
            pModel->Draw(mode, shader);
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
            pModel->Draw(mode, shader);
            boundingVolume.get()->drawAABB(transform.getModelMatrix());
            display++;
        }
    }
};