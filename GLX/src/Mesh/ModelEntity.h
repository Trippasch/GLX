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
            material.getShader().Use().SetVector3f("material.albedo", material.getAlbedo());
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
            if (ImGui::DragFloat3("Position", (float*)&transform.getLocalPosition(), 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
                transform.setLocalPosition(transform.getLocalPosition());
            }
            if (ImGui::DragFloat3("Rotation", (float*)&transform.getLocalRotation(), 0.01f, -180.0f, 180.0f, "%.2f")) {
                transform.setLocalRotation(transform.getLocalRotation());
            }
            if (ImGui::DragFloat3("Scale", (float*)&transform.getLocalScale(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                transform.setLocalScale(transform.getLocalScale());
            }
            if (ImGui::TreeNodeEx("Material")) {
                if (ImGui::ColorEdit3("Albedo", (float*)&material.getAlbedo())) {
                    material.setAlbedo(material.getAlbedo());
                }
                if (ImGui::SliderFloat("Metallic", (float*)&material.getMetallic(), 0.0f, 1.0f, "%.2f")) {
                    material.setMetallic(material.getMetallic());
                }
                if (ImGui::SliderFloat("Roughness", (float*)&material.getRoughness(), 0.0f, 1.0f, "%.2f")) {
                    material.setRoughness(material.getRoughness());
                }
                if (ImGui::SliderFloat("AO", (float*)&material.getAO(), 0.0f, 1.0f, "%.2f")) {
                    material.setAO(material.getAO());
                }
                if (ImGui::DragFloat("Emissive", (float*)&material.getEmissive(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                    material.setEmissive(material.getEmissive());
                }
                ImGui::TreePop();
            }
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