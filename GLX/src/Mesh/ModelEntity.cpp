#include "Mesh/ModelEntity.h"

ModelEntity::ModelEntity(Model& model, RendererLayer* renderer, bool hasAnimation)
    : pModel(&model), m_Renderer(renderer), m_HasAnimation(hasAnimation)
{
    boundingVolume = std::make_shared<AABB>(generateAABB(model));
    boundingVolume->drawAABBSetup();

    if (hasAnimation) {
        m_Animator = new Animator();
        m_Animator->AddAnimation("run", &ResourceManager::GetAnimation("vampire_standing_run_forward"));
        m_Animator->AddAnimation("idle", &ResourceManager::GetAnimation("vampire_standing_idle"));
    }
}

AABB ModelEntity::generateAABB(const Model& model)
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

std::vector<glm::vec3> ModelEntity::transformVertices(const Model& model, const std::vector<glm::mat4>& boneMatrices)
{
    std::vector<glm::vec3> transformedVertices;
    for (auto&& mesh : model.meshes) {
        for (auto&& vertex : mesh.vertices) {
            glm::vec3 transformedVertex = glm::vec3(0.0f);
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                int boneID = vertex.BoneIDs[i];
                if (boneID >= boneMatrices.size() || boneID < 0) {
                    continue;
                }
                float weight = vertex.Weights[i];
                transformedVertex += weight * glm::vec3(boneMatrices[boneID] * glm::vec4(vertex.Position, 1.0f));
            }
            transformedVertices.push_back(transformedVertex);
        }
    }
    return transformedVertices;
}

void ModelEntity::updateAABB(const Model& model, AABB& aabb)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    std::vector<glm::vec3> transformedVertices = transformVertices(model, m_Animator->GetFinalBoneMatrices());

    for (auto&& transformedVertex : transformedVertices) {
        minAABB.x = std::min(minAABB.x, transformedVertex.x);
        minAABB.y = std::min(minAABB.y, transformedVertex.y);
        minAABB.z = std::min(minAABB.z, transformedVertex.z);

        maxAABB.x = std::max(maxAABB.x, transformedVertex.x);
        maxAABB.y = std::max(maxAABB.y, transformedVertex.y);
        maxAABB.z = std::max(maxAABB.z, transformedVertex.z);
    }
    aabb.center = (maxAABB + minAABB) * 0.5f;
    aabb.extents = { maxAABB.x - aabb.center.x, maxAABB.y - aabb.center.y, maxAABB.z - aabb.center.z };
}

SphereBV ModelEntity::generateSphereBV(const Model& model)
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

void ModelEntity::Destroy()
{
    if (pModel) {
        for (const auto &mesh : pModel->meshes)
            mesh.Destroy();
        for (const auto &tex : pModel->textures_loaded)
            tex.Destroy();
        pModel = nullptr;
    }
    if (m_HasAnimation) {
        delete m_Animator;
        m_Animator = nullptr;
    }
}

void ModelEntity::drawSelfAndChildSimple(GLenum& mode, Shader& shader)
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
        if (m_Animate) {
            const auto &transforms = m_Animator->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); i++) {
                m_Renderer->GetMatricesUBO().FillBuffer(&transforms[i], sizeof(glm::mat4) + i * sizeof(glm::mat4), sizeof(glm::mat4));
            }
        }
        m_Renderer->GetMatricesUBO().FillBuffer(&m_Animate, sizeof(glm::mat4) + sizeof(glm::mat4) * MAX_BONES, sizeof(bool));
        shader.Use().SetMatrix4(1, transform.getModelMatrix());
        pModel->Draw(mode, shader);
    }
}

void ModelEntity::drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total)
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
        if (m_Animate) {
            updateAABB(*pModel, *boundingVolume);
            boundingVolume->drawAABBSetup();

            m_Animator->UpdateAnimation(m_Renderer->GetDeltaTime());
            const auto &transforms = m_Animator->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); i++) {
                m_Renderer->GetMatricesUBO().FillBuffer(&transforms[i], sizeof(glm::mat4) + i * sizeof(glm::mat4), sizeof(glm::mat4));
            }
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
            m_Renderer->GetMatricesUBO().FillBuffer(&m_Animate, sizeof(glm::mat4) + sizeof(glm::mat4) * MAX_BONES, sizeof(bool));
            material.getShader().Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, material.getShader());

            if (drawAABB) {
                boundingVolume->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }
}

void ModelEntity::drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total)
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
        if (m_Animate) {
            updateAABB(*pModel, *boundingVolume);
            boundingVolume->drawAABBSetup();

            m_Animator->UpdateAnimation(m_Renderer->GetDeltaTime());
            const auto &transforms = m_Animator->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); i++) {
                m_Renderer->GetMatricesUBO().FillBuffer(&transforms[i], sizeof(glm::mat4) + i * sizeof(glm::mat4), sizeof(glm::mat4));
            }
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
            m_Renderer->GetMatricesUBO().FillBuffer(&m_Animate, sizeof(glm::mat4) + sizeof(glm::mat4) * MAX_BONES, sizeof(bool));
            material.getShader().Use().SetMatrix4(1, transform.getModelMatrix());
            pModel->Draw(mode, material.getShader());

            if (drawAABB) {
                boundingVolume->drawAABB(transform.getModelMatrix());
            }

            display++;
        }
    }
}

void ModelEntity::renderGUI(int i)
{
    if (ImGui::TreeNodeEx(("Model " + std::to_string(i)).c_str())) {
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

        if (m_HasAnimation) {
            if (ImGui::TreeNodeEx("Animator", base_flags)) {
                    ImGui::Combo("Animation", &m_CurrentAnimation, m_Animations, IM_ARRAYSIZE(m_Animations));
                    switch (m_CurrentAnimation) {
                        case 0:
                            m_Animate = false;
                            boundingVolume = std::make_shared<AABB>(generateAABB(*pModel));
                            boundingVolume->drawAABBSetup();
                            break;
                        case 1:
                            m_Animate = true;
                            m_Animator->SetAnimation("idle");
                            break;
                        case 2:
                            m_Animate = true;
                            m_Animator->SetAnimation("run");
                            break;
                    }
                ImGui::TreePop();
            }
        }

        if (ImGui::Button("Remove Model", ImVec2(0, 0))) {
            Destroy();
            parent->children.erase(parent->children.begin() + i);
        }

        ImGui::TreePop();
    }
}