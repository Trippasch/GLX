#pragma once

#include "Mesh/Entity.h"

class Sphere : public Entity
{
public:
    VertexBuffer* pVBO = nullptr;
    IndexBuffer* pEBO = nullptr;
    GLuint pIndexCount;

    Sphere() : pVBO(nullptr), pEBO(nullptr), pIndexCount(0)
    {}

    Sphere(VertexBuffer& vbo, float* vertices, size_t size, IndexBuffer& ebo, GLuint indexCount)
        : pVBO(&vbo), pEBO(&ebo), pIndexCount(indexCount)
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
            pEBO->Bind();
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawElements(mode, pIndexCount, GL_UNSIGNED_INT, 0);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);
            pEBO->UnBind();
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
        else if (pVBO != nullptr && boundingVolume->isOnFrustum(frustum, transform)) {
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
            pEBO->Bind();
            pVBO->LinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
            pVBO->LinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            pVBO->LinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glDrawElements(mode, pIndexCount, GL_UNSIGNED_INT, 0);
            pVBO->UnlinkAttrib(0);
            pVBO->UnlinkAttrib(1);
            pVBO->UnlinkAttrib(2);
            pEBO->UnBind();

            if (drawAABB) {
                boundingVolume.get()->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }

    void renderGUI(int i) override
    {
        if (ImGui::TreeNodeEx(("Sphere " + std::to_string(i)).c_str())) {
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
                    material.setShader(ResourceManager::GetShader("pbr_lighting_textured"));
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