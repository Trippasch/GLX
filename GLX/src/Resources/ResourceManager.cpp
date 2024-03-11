#include "Resources/ResourceManager.h"
#include "Core/Log.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image.h>

// Instantiate static variables
std::unordered_map<std::string, Texture2D> ResourceManager::Textures;
std::unordered_map<std::string, Shader> ResourceManager::Shaders;
std::unordered_map<std::string, Model> ResourceManager::Models;

Model ResourceManager::LoadModel(const char *file, const std::string &name)
{
    Models[name] = loadModelFromFile(file);
    return Models[name];
}

Model& ResourceManager::GetModel(const std::string &name)
{
    return Models[name];
}

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::string &name, const std::vector<std::string> &defines)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile, defines);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(const std::string &name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, const std::string &name, bool gamma)
{
    Textures[name] = loadTextureFromFile(file, gamma);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(const std::string &name)
{
    return Textures[name];
}

Texture2D ResourceManager::LoadHDRTexture(const char *file, const std::string &name)
{
    Textures[name] = loadHDRTextureFromFile(file);
    return Textures[name];
}

Texture2D& ResourceManager::GetHDRTexture(const std::string &name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (const auto &shader : Shaders)
        glDeleteProgram(shader.second.ID);
    // (properly) delete all textures
    for (const auto &texture : Textures)
        glDeleteTextures(1, &texture.second.ID);
    // (properly) delete all meshes data
    for (const auto &model : Models) {
        auto meshes = model.second.meshes;
        for (const auto &mesh : meshes)
            mesh.Destroy();
        for (const auto &tex : model.second.textures_loaded)
            tex.Destroy();
    }
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::vector<std::string> &defines)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        GL_ERROR("ERROR::SHADER: Failed to read shader files");
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr, defines);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool gamma)
{
    // create texture object
    Texture2D texture;

    // load image
    int width, height, nrChannels;
    void* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrChannels == 1)
            internalFormat = dataFormat = GL_RED;
        else if (nrChannels == 3) {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 4) {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        texture.Internal_Format = internalFormat;
        texture.Image_Format = dataFormat;
        // now generate texture
        texture.Generate(width, height, data, GL_TRUE);
        // and finally free image data
        stbi_image_free(data);
    }
    else {
        GL_ERROR("Texture failed to load at path: {0} ", file);
        stbi_image_free(data);
    }
    return texture;
}

Texture2D ResourceManager::loadHDRTextureFromFile(const char *file)
{
    // create texture object
    Texture2D texture;

    // load image
    stbi_set_flip_vertically_on_load(1);
    int width, height, nrChannels;
    float* data = stbi_loadf(file, &width, &height, &nrChannels, 0);
    if (data) {
        texture.Internal_Format = GL_RGB16F;
        texture.Image_Format = GL_RGB;
        texture.Wrap_S = GL_CLAMP_TO_EDGE;
        texture.Wrap_T = GL_CLAMP_TO_EDGE;
        texture.Data_Type = GL_FLOAT;
        // now generate texture
        texture.Generate(width, height, data);
        // and finally free image data
        stbi_image_free(data);
    }
    else {
        GL_ERROR("HDR Texture failed to load at path: {0} ", file);
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(0);
    return texture;
}

Model ResourceManager::loadModelFromFile(const char *file)
{
    Model model(file);
    return model;
}