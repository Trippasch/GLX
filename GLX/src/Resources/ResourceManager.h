#pragma once

#include <unordered_map>
#include <string>

#include <glad/glad.h>

#include "Resources/Texture2D.h"
#include "Resources/Shader.h"
#include "Resources/Model.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
    // resource storage
    static std::unordered_map<std::string, Shader> Shaders;
    static std::unordered_map<std::string, Texture2D> Textures;
    static std::unordered_map<std::string, Model> Models;

    // loads (and generates) a model from file
    static Model LoadModel(const char *file, const std::string &name);
    // retrieves a stored model
    static Model& GetModel(const std::string &name);
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::string &name, const std::vector<std::string> &defines = {});
    // retrieves a stored shader
    static Shader& GetShader(const std::string &name);
    // loads (and generates) a texture from file
    static Texture2D LoadTexture(const char *file, const std::string &name, bool gamma = false);
    // retrieves a stored texture
    static Texture2D& GetTexture(const std::string &name);
    // loads (and generates) an HDR texture from file
    static Texture2D LoadHDRTexture(const char *file, const std::string &name);
    // retrieves a stored HDR texture
    static Texture2D& GetHDRTexture(const std::string &name);
    // properly de-allocates all loaded resources
    static void Clear();
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads and generates a model from file
    static Model loadModelFromFile(const char *file);
    // loads and generates a shader from file
    static Shader loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr, const std::vector<std::string> &defines = {});
    // loads a single texture from file
    static Texture2D loadTextureFromFile(const char *file, bool gamma);
    // loads a single HDR texture from file
    static Texture2D loadHDRTextureFromFile(const char *file);
};