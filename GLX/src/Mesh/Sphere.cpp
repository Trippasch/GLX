#include "Mesh/Sphere.h"

Sphere::Sphere(VertexBuffer& vbo, float* vertices, size_t size, IndexBuffer& ebo, GLuint indexCount, RendererLayer* renderer)
    : pVBO(&vbo), pEBO(&ebo), pIndexCount(indexCount), m_Renderer(renderer)
{
    boundingVolume = std::make_shared<AABB>(generateAABB(vertices, size));
    boundingVolume.get()->drawAABBSetup();
}

AABB Sphere::generateAABB(const float* vertices, size_t size)
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

SphereBV Sphere::generateSphereBV(const float* vertices, size_t size)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    for (int i = 0; i < size; i += 8) {
        glm::vec3 vertexPosition = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);

        minAABB.x = std::min(minAABB.x, vertexPosition.x);
        minAABB.y = std::min(minAABB.y, vertexPosition.y);
        minAABB.z = std::min(minAABB.z, vertexPosition.z);

        maxAABB.x = std::max(maxAABB.x, vertexPosition.x);
        maxAABB.y = std::max(maxAABB.y, vertexPosition.y);
        maxAABB.z = std::max(maxAABB.z, vertexPosition.z);
    }

    return SphereBV((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}

void Sphere::drawSelfAndChildSimple(GLenum& mode, Shader& shader)
{
    if (!children.empty()) {
        for (auto&& child : children) {
            child->drawSelfAndChildSimple(mode, shader);
        }
    }
    else {
        if (material.getIsTranslucent()) {
            return;
        }
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

void Sphere::drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total)
{
    if (!children.empty()) {
        total++;
        for (auto&& child : children) {
            child->drawSelfAndChildTranslucent(mode, frustum, display, total);
        }
    }
    else {
        if (!material.getIsTranslucent()) {
            return;
        }
        if (boundingVolume->isOnFrustum(frustum, transform)) {
            if (material.getIsTextured()) {
                material.getAlbedoTexture().Bind(GL_TEXTURE_2D, 3);
                material.getNormalTexture().Bind(GL_TEXTURE_2D, 4);
                material.getMetallicTexture().Bind(GL_TEXTURE_2D, 5);
                material.getRoughnessTexture().Bind(GL_TEXTURE_2D, 6);
                material.getSpecularTexture().Bind(GL_TEXTURE_2D, 7);
                material.getAOTexture().Bind(GL_TEXTURE_2D, 8);
            }
            material.getEmissiveTexture().Bind(GL_TEXTURE_2D, 9);
            m_Renderer->GetObjectUBO().FillBuffer(&material.getObjectID(), sizeof(float), sizeof(float));
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
                boundingVolume->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }
}

void Sphere::drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total)
{
    if (!children.empty()) {
        total++;
        for (auto&& child : children) {
            child->drawSelfAndChild(mode, frustum, display, total);
        }
    }
    else {
        if (material.getIsTranslucent()) {
            return;
        }
        if (boundingVolume->isOnFrustum(frustum, transform)) {
            if (material.getIsTextured()) {
                material.getAlbedoTexture().Bind(GL_TEXTURE_2D, 3);
                material.getNormalTexture().Bind(GL_TEXTURE_2D, 4);
                material.getMetallicTexture().Bind(GL_TEXTURE_2D, 5);
                material.getRoughnessTexture().Bind(GL_TEXTURE_2D, 6);
                material.getSpecularTexture().Bind(GL_TEXTURE_2D, 7);
                material.getAOTexture().Bind(GL_TEXTURE_2D, 8);
            }
            material.getEmissiveTexture().Bind(GL_TEXTURE_2D, 9);
            m_Renderer->GetObjectUBO().FillBuffer(&material.getObjectID(), sizeof(float), sizeof(float));
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
                boundingVolume->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }
}

void Sphere::renderGUI(int i)
{
    if (ImGui::TreeNodeEx(("Sphere " + std::to_string(i)).c_str())) {
        transform.renderTransformGUI();
        material.renderMaterialGUI(m_Renderer->GetObjectUBO());

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

        if (ImGui::Button("Remove Sphere", ImVec2(0, 0))) {
            parent->children.erase(parent->children.begin() + i);
        }

        ImGui::TreePop();
    }
}