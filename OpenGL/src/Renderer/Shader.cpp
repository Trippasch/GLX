#include "Renderer/Shader.h"

#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Log.h"

// #include "helper_gl.h"

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void Shader::UnBind() const
{
    glUseProgram(0);
    // SDK_CHECK_ERROR_GL();
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    // SDK_CHECK_ERROR_GL();
}

void Shader::SetUniform1i(const std::string& name, const GLint value)
{
    glUniform1i(GetUniformLocation(name), value);
    // SDK_CHECK_ERROR_GL();
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                // set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                // set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

GLuint Shader::CompileShader(GLenum type, const std::string& source)
{
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        GL_ERROR("Failed to compile {0} shader!", GL_VERTEX_SHADER ? "vertex" : "fragment")
        GL_INFO(message);
        glDeleteShader(id);
        return 0;
    }

    // SDK_CHECK_ERROR_GL();
    return id;
}

GLuint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // SDK_CHECK_ERROR_GL();
    return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
    auto location_search = m_UniformLocationCache.find(name);
    if (location_search != m_UniformLocationCache.end())
    {
        return location_search->second;
    }

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        GL_WARN("Warning: uniform {0} doesn't exist!", name);
    }

    m_UniformLocationCache[name] = location;

    // SDK_CHECK_ERROR_GL();
    return location;
}
