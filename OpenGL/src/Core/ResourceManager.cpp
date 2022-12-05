#include "Core/ResourceManager.h"

#include "Core/Log.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image.h>

// Instantiate static variables
std::unordered_map<std::string, Texture2D> ResourceManager::Textures;
std::unordered_map<std::string, Shader> ResourceManager::Shaders;
std::unordered_map<std::string, Model> ResourceManager::Models;

Model ResourceManager::LoadModel(const char *file, std::string name)
{
    Models[name] = loadModelFromFile(file);
    return Models[name];
}

Model& ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
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

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
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
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_SRGB_ALPHA;
        texture.Image_Format = GL_RGBA;
    }
    else {
        texture.Internal_Format = GL_SRGB;
    }
    // load image
    int width, height, nrChannels;
    void* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data, GL_TRUE);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}

Model ResourceManager::loadModelFromFile(const char *file)
{
    Model model(file);
    return model;
}