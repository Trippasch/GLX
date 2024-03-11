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
        else if (pVBO != nullptr) {
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

    void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, UniformBuffer& objectUBO, unsigned int& display, unsigned int& total) override
    {
        if (!children.empty()) {
            total++;
            for (auto&& child : children) {
                child->drawSelfAndChildTranslucent(mode, frustum, objectUBO, display, total);
            }
        }
        else if (pVBO != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
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
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawArrays(mode, 0, 36);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);

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
        else if (pVBO != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
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
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawArrays(mode, 0, 36);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);

            if (drawAABB) {
                boundingVolume.get()->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }

    void renderGUI(int i, UniformBuffer& objectUBO) override
    {
        if (ImGui::TreeNodeEx(("Cube " + std::to_string(i)).c_str())) {
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
            if (material.getIsTranslucent()) {
                defines.push_back("TRANSLUCENT");
            }

            if (definesChanged) {
                ResourceManager::LoadShader("assets/shaders/pbrVS.glsl", "assets/shaders/pbrFS.glsl", nullptr, "pbr_lighting", defines);
                material.setShader(ResourceManager::GetShader("pbr_lighting"));
            }

            ImGui::Checkbox("Draw AABB", &drawAABB);
            if (ImGui::Button("Remove Cube", ImVec2(0, 0))) {
                parent->children.erase(parent->children.begin() + i);
            }

            ImGui::TreePop();
        }
    }
};