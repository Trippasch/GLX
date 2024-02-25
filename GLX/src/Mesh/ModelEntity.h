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
        else if (pModel != nullptr) {
            shader.Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, shader);
        }
    }

    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) override
    {
        if (!children.empty()) {
            total++;
            for (auto&& child : children) {
                child->drawSelfAndChild(mode, frustum, display, total);
            }
        }
        else if (pModel != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
            if (material.getIsTextured()) {
                material.getAlbedoTexture().Bind(3);
                material.getNormalTexture().Bind(4);
                material.getMetallicTexture().Bind(5);
                material.getRoughnessTexture().Bind(6);
                material.getAOTexture().Bind(7);
            }
            material.getEmissiveTexture().Bind(8);
            material.getShader().Use().SetVector4f("material.albedo", material.getAlbedo());
            material.getShader().Use().SetFloat("material.metallic", material.getMetallic());
            material.getShader().Use().SetFloat("material.roughness", material.getRoughness());
            material.getShader().Use().SetFloat("material.ao", material.getAO());
            material.getShader().Use().SetFloat("material.emissive", material.getEmissive());
            material.getShader().Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, material.getShader());

            if (drawAABB) {
                boundingVolume.get()->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }

    void renderGUI(int i) override
    {
        if (ImGui::TreeNodeEx(("Model " + std::to_string(i)).c_str())) {
            transform.renderTransformGUI();
            material.renderMaterialGUI();
            if (ImGui::Checkbox("Textured", &material.getIsTextured())) {
                if (material.getIsTextured()) {
                    if (isGLTF) {
                        material.setShader(ResourceManager::GetShader("pbr_lighting_textured_gltf"));
                    }
                    else {
                        material.setShader(ResourceManager::GetShader("pbr_lighting_textured"));
                    }
                }
                else {
                    material.setShader(ResourceManager::GetShader("pbr_lighting"));
                }
            }
            ImGui::Checkbox("Draw AABB", &drawAABB);
            ImGui::TreePop();
        }
    }
};