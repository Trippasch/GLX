#pragma once

#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Resources/UniformBuffer.h"

#include <glm/glm.hpp>

class Material
{
protected:
    glm::vec4 m_Albedo = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_Metallic = 0.0f;
    float m_Roughness = 0.0f;
    float m_AO = 1.0f;
    float m_Emissive = 0.0f;
    bool m_IsTextured = false;
    bool m_IsTranslucent = false;
    int m_ObjectID = 0;
    Shader m_Shader;
    Texture2D m_AlbedoTexture;
    Texture2D m_NormalTexture;
    Texture2D m_MetallicTexture;
    Texture2D m_RoughnessTexture;
    Texture2D m_AOTexture;
    Texture2D m_EmissiveTexture;

public:
    static const GLuint MAX_OBJECTS = 100;

    void renderMaterialGUI(UniformBuffer& objectUBO)
    {
        if (ImGui::TreeNodeEx("Material")) {
            int materialOffset = sizeof(glm::vec4) + getObjectID() * 2 * sizeof(glm::vec4);
            if (ImGui::ColorEdit4("Albedo", (float*)&getAlbedo())) {
                setAlbedo(getAlbedo());
                objectUBO.FillBuffer(&getAlbedo(), materialOffset, sizeof(glm::vec4));
            }
            if (ImGui::SliderFloat("Metallic", (float*)&getMetallic(), 0.0f, 1.0f, "%.2f")) {
                setMetallic(getMetallic());
                objectUBO.FillBuffer(&getMetallic(), materialOffset + sizeof(glm::vec4), sizeof(float));
            }
            if (ImGui::SliderFloat("Roughness", (float*)&getRoughness(), 0.0f, 1.0f, "%.2f")) {
                setRoughness(getRoughness());
                objectUBO.FillBuffer(&getRoughness(), materialOffset + sizeof(glm::vec4) + sizeof(float), sizeof(float));
            }
            if (ImGui::SliderFloat("AO", (float*)&getAO(), 0.0f, 1.0f, "%.2f")) {
                setAO(getAO());
                objectUBO.FillBuffer(&getAO(), materialOffset + sizeof(glm::vec4) + 2*sizeof(float), sizeof(float));
            }
            if (ImGui::DragFloat("Emissive", (float*)&getEmissive(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                setEmissive(getEmissive());
                objectUBO.FillBuffer(&getEmissive(), materialOffset + sizeof(glm::vec4) + 3*sizeof(float), sizeof(float));
            }
            ImGui::TreePop();
        }
    }

    void fillMaterialBuffer(UniformBuffer& objectUBO, int objectID)
    {
        setObjectID(objectID);
        int materialOffset = sizeof(glm::vec4) + objectID * 2 * sizeof(glm::vec4);
        objectUBO.FillBuffer(&getAlbedo(), materialOffset, sizeof(glm::vec4));
        objectUBO.FillBuffer(&getMetallic(), materialOffset + sizeof(glm::vec4), sizeof(float));
        objectUBO.FillBuffer(&getRoughness(), materialOffset + sizeof(glm::vec4) + sizeof(float), sizeof(float));
        objectUBO.FillBuffer(&getAO(), materialOffset + sizeof(glm::vec4) + 2*sizeof(float), sizeof(float));
        objectUBO.FillBuffer(&getEmissive(), materialOffset + sizeof(glm::vec4) + 3*sizeof(float), sizeof(float));
    }

    const glm::vec4& getAlbedo() const
    {
        return m_Albedo;
    }
    const float& getMetallic() const
    {
        return m_Metallic;
    }
    const float& getRoughness() const
    {
        return m_Roughness;
    }
    const float& getAO() const
    {
        return m_AO;
    }
    const float& getEmissive() const
    {
        return m_Emissive;
    }
    bool& getIsTextured()
    {
        return m_IsTextured;
    }
    bool& getIsTranslucent()
    {
        return m_IsTranslucent;
    }
    int& getObjectID()
    {
        return m_ObjectID;
    }
    Shader& getShader()
    {
        return m_Shader;
    }
    Texture2D& getAlbedoTexture()
    {
        return m_AlbedoTexture;
    }
    Texture2D& getNormalTexture()
    {
        return m_NormalTexture;
    }
    Texture2D& getMetallicTexture()
    {
        return m_MetallicTexture;
    }
    Texture2D& getRoughnessTexture()
    {
        return m_RoughnessTexture;
    }
    Texture2D& getAOTexture()
    {
        return m_AOTexture;
    }
    Texture2D& getEmissiveTexture()
    {
        return m_EmissiveTexture;
    }

    void setAlbedo(const glm::vec4& newAlbedo)
    {
        m_Albedo = newAlbedo;
    }
    void setMetallic(const float& newMetallic)
    {
        m_Metallic = newMetallic;
    }
    void setRoughness(const float& newRoughness)
    {
        m_Roughness = newRoughness;
    }
    void setAO(const float& newAO)
    {
        m_AO = newAO;
    }
    void setEmissive(const float& newEmissive)
    {
        m_Emissive = newEmissive;
    }
    void setIsTextured(const bool& newIsTextured)
    {
        m_IsTextured = newIsTextured;
    }
    void setIsTranslucent(const bool& newIsTranslucent)
    {
        m_IsTranslucent = newIsTranslucent;
    }
    void setObjectID(const int& newObjectID)
    {
        m_ObjectID = newObjectID;
    }
    void setShader(const Shader& newShader)
    {
        m_Shader = newShader;
    }
    void setAlbedoTexture(const Texture2D& newAlbedoTexture)
    {
        m_AlbedoTexture = newAlbedoTexture;
    }
    void setNormalTexture(const Texture2D& newNormalTexture)
    {
        m_NormalTexture = newNormalTexture;
    }
    void setMetallicTexture(const Texture2D& newMetallicTexture)
    {
        m_MetallicTexture = newMetallicTexture;
    }
    void setRoughnessTexture(const Texture2D& newRoughnessTexture)
    {
        m_RoughnessTexture = newRoughnessTexture;
    }
    void setAOTexture(const Texture2D& newAOTexture)
    {
        m_AOTexture = newAOTexture;
    }
    void setEmissiveTexture(const Texture2D& newEmissiveTexture)
    {
        m_EmissiveTexture = newEmissiveTexture;
    }
};