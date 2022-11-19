#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
public:
    Shader() : m_RendererID(0), m_FilePath(""), m_UniformLocationCache(0) {}
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void UnBind() const;

    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void SetUniform1i(const std::string& name, const GLint value);

private:
    GLuint m_RendererID;
    std::string m_FilePath;
    std::unordered_map<std::string, int> m_UniformLocationCache;

private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    GLuint CompileShader(GLenum type, const std::string& source);
    GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    int GetUniformLocation(const std::string& name);

};
