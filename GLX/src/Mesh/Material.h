#pragma once

#include "Resources/Shader.h"
#include "Resources/Texture2D.h"

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
    Shader m_Shader;
    Texture2D m_AlbedoTexture;
    Texture2D m_NormalTexture;
    Texture2D m_MetallicTexture;
    Texture2D m_RoughnessTexture;
    Texture2D m_AOTexture;
    Texture2D m_EmissiveTexture;

public:
    void renderMaterialGUI()
    {
        if (ImGui::TreeNodeEx("Material")) {
            if (ImGui::ColorEdit4("Albedo", (float*)&getAlbedo())) {
                setAlbedo(getAlbedo());
            }
            if (ImGui::SliderFloat("Metallic", (float*)&getMetallic(), 0.0f, 1.0f, "%.2f")) {
                setMetallic(getMetallic());
            }
            if (ImGui::SliderFloat("Roughness", (float*)&getRoughness(), 0.0f, 1.0f, "%.2f")) {
                setRoughness(getRoughness());
            }
            if (ImGui::SliderFloat("AO", (float*)&getAO(), 0.0f, 1.0f, "%.2f")) {
                setAO(getAO());
            }
            if (ImGui::DragFloat("Emissive", (float*)&getEmissive(), 0.01f, 0.0f, FLT_MAX, "%.2f")) {
                setEmissive(getEmissive());
            }
            ImGui::TreePop();
        }
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