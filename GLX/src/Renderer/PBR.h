#pragma once

#include "Resources/Texture2D.h"
#include "Resources/FrameBuffer.h"
#include "Resources/ResourceManager.h"
#include "Renderer/RendererLayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RendererLayer;

class PBR
{
private:
    RendererLayer* m_Renderer;

    FrameBuffer m_CaptureFBO;

    Texture2D m_EnvCubemap;
    GLuint m_EnvCubemapWidth = 2048;
    GLuint m_EnvCubemapHeight = 2048;

    Texture2D m_Irradiancemap;
    GLuint m_IrradiancemapWidth = 32;
    GLuint m_IrradiancemapHeight = 32;

    Texture2D m_Prefiltermap;
    GLuint m_PrefiltermapWidth = 512;
    GLuint m_PrefiltermapHeight = 512;

    Texture2D m_BRDFLUTTexture;
    GLuint m_BRDFLUTTextureWidth = 512;
    GLuint m_BRDFLUTTextureHeight = 512;

    std::string m_SkyboxFilename = "assets/textures/hdr-skyboxes/Nature_8K_hdri.jpg";

public:
    PBR(RendererLayer* renderer);
    ~PBR();
    void CreateSkybox();
    void RenderSkybox();
    void RenderSkyboxGUI();

    const Texture2D& GetEnvCubemap() const { return m_EnvCubemap; }
    const Texture2D& GetIrradiancemap() const { return m_Irradiancemap; }
    const Texture2D& GetPrefiltermap() const { return m_Prefiltermap; }
    const Texture2D& GetBRDFLUTTexture() const { return m_BRDFLUTTexture; }
};