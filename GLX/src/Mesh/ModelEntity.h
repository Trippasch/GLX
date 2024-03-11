#pragma once

#include "Mesh/Entity.h"

class ModelEntity : public Entity
{
private:
    Model* pModel = nullptr;

public:
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

    void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, UniformBuffer& objectUBO, unsigned int& display, unsigned int& total) override
    {
        if (!children.empty()) {
            total++;
            for (auto&& child : children) {
                child->drawSelfAndChildTranslucent(mode, frustum, objectUBO, display, total);
            }
        }
        else if (pModel != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
            if (!material.getIsTranslucent()) {
                return;
            }
            if (material.getIsTextured()) {
                material.getAlbedoTexture().Bind(GL_TEXTURE_2D, 3);
                material.getNormalTexture().Bind(GL_TEXTURE_2D, 4);
                material.getMetallicTexture().Bind(GL_TEXTURE_2D, 5);
                material.getRoughnessTexture().Bind(GL_TEXTURE_2D, 6);
                material.getAOTexture().Bind(GL_TEXTURE_2D, 7);
            }
            material.getEmissiveTexture().Bind(GL_TEXTURE_2D, 8);
            objectUBO.FillBuffer(&material.getObjectID(), sizeof(float), sizeof(float));
            material.getShader().Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, material.getShader());

            if (drawAABB) {
                boundingVolume.get()->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }

    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, UniformBuffer& objectUBO, unsigned int& display, unsigned int& total) override
    {
        if (!children.empty()) {
            total++;
            for (auto&& child : children) {
                child->drawSelfAndChild(mode, frustum, objectUBO, display, total);
            }
        }
        else if (pModel != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
            if (material.getIsTranslucent()) {
                return;
            }
            if (material.getIsTextured()) {
                material.getAlbedoTexture().Bind(GL_TEXTURE_2D, 3);
                material.getNormalTexture().Bind(GL_TEXTURE_2D, 4);
                material.getMetallicTexture().Bind(GL_TEXTURE_2D, 5);
                material.getRoughnessTexture().Bind(GL_TEXTURE_2D, 6);
                material.getAOTexture().Bind(GL_TEXTURE_2D, 7);
            }
            material.getEmissiveTexture().Bind(GL_TEXTURE_2D, 8);
            objectUBO.FillBuffer(&material.getObjectID(), sizeof(float), sizeof(float));
            material.getShader().Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, material.getShader());

            if (drawAABB) {
                boundingVolume.get()->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }

    void renderGUI(int i, UniformBuffer& objectUBO) override
    {
        if (ImGui::TreeNodeEx(("Model " + std::to_string(i)).c_str())) {
            transform.renderTransformGUI();
            material.renderMaterialGUI(objectUBO);

            bool definesChanged = false;

            if (ImGui::Checkbox("Textured", &material.getIsTextured())) {
                definesChanged = true;
            }
            if (ImGui::Checkbox("Translucent", &material.getIsTranslucent())) {
                definesChanged = true;
            }

            std::vector<std::string> defines = { "MAX_DIR_LIGHTS 10", "MAX_POINT_LIGHTS 10", "MAX_OBJECTS 100" };
            if (material.getIsTextured()) {
                defines.push_back("TEXTURED");
            }
            if (isGLTF) {
                defines.push_back("GLTF");
            }
            if (material.getIsTranslucent()) {
                defines.push_back("TRANSLUCENT");
            }

            if (definesChanged) {
                ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", defines);
                material.setShader(ResourceManager::GetShader("pbr_lighting"));
            }

            ImGui::Checkbox("Draw AABB", &drawAABB);
            if (ImGui::Button("Remove Model", ImVec2(0, 0))) {
                parent->children.erase(parent->children.begin() + i);
            }

            ImGui::TreePop();
        }
    }
};