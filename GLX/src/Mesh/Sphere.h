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
                material.getAlbedoTexture().Bind(GL_TEXTURE_2D, 3);
                material.getNormalTexture().Bind(GL_TEXTURE_2D, 4);
                material.getMetallicTexture().Bind(GL_TEXTURE_2D, 5);
                material.getRoughnessTexture().Bind(GL_TEXTURE_2D, 6);
                material.getAOTexture().Bind(GL_TEXTURE_2D, 7);
            }
            material.getEmissiveTexture().Bind(GL_TEXTURE_2D, 8);
            material.getShader().Use().SetVector4f("material.albedo", material.getAlbedo());
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
            transform.renderTransformGUI();
            material.renderMaterialGUI();
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