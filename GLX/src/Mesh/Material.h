#pragma once

#include <glm/glm.hpp>

class Material
{
protected:
    glm::vec3 m_Albedo = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_Metallic = 0.0f;
    float m_Roughness = 0.0f;
    float m_AO = 1.0f;

public:
    const glm::vec3& getAlbedo() const
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

    void setAlbedo(const glm::vec3& newAlbedo)
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
};