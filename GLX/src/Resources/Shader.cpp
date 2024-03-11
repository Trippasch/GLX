#include "Resources/Shader.h"

#include <iostream>

#include "Core/Log.h"

// #include "helper_gl.h"

Shader &Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}

void Shader::Compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource, const std::vector<std::string> &defines)
{
    unsigned int sVertex, sFragment, gShader;
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    this->ID = glCreateProgram();

    if (defines.size() > 0) {
        std::vector<std::string> vsMergedCode;
        std::vector<std::string> fsMergedCode;
        std::string vsCode = vertexSource;
        std::string fsCode = fragmentSource;
        // first determine if the user supplied a #version  directive at the top of the shader 
        // code, in which case we  extract it and add it 'before' the list of define code.
        // the GLSL version specifier is only valid as the first line of the GLSL code; 
        // otherwise the GLSL version defaults to 1.1.
        std::string firstLine = vsCode.substr(0, vsCode.find("\n"));
        if (firstLine.find("#version") != std::string::npos)
        {
            // strip shader code of first line and add to list of shader code strings.
            vsCode = vsCode.substr(vsCode.find("\n") + 1, vsCode.length() - 1);
            vsMergedCode.push_back(firstLine + "\n");
        }
        firstLine = fsCode.substr(0, fsCode.find("\n"));
        if (firstLine.find("#version") != std::string::npos)
        {
            // strip shader code of first line and add to list of shader code strings.
            fsCode = fsCode.substr(fsCode.find("\n") + 1, fsCode.length() - 1);
            fsMergedCode.push_back(firstLine + "\n");
        }
        // then add define statements to the shader string list.
        for (unsigned int i = 0; i < defines.size(); ++i)
        {
            std::string define = "#define " + defines[i] + "\n";
            vsMergedCode.push_back(define);
            fsMergedCode.push_back(define);
        }
        // then addremaining shader code to merged result and pass result to glShaderSource.
        vsMergedCode.push_back(vsCode);
        fsMergedCode.push_back(fsCode);
        // note that we manually build an array of C style  strings as glShaderSource doesn't 
        // expect it in any other format.
        // all strings are null-terminated so pass NULL as glShaderSource's final argument.
        const char **vsStringsC = new const char*[vsMergedCode.size()];
        const char **fsStringsC = new const char*[fsMergedCode.size()];
        for (unsigned int i = 0; i < vsMergedCode.size(); ++i)
            vsStringsC[i] = vsMergedCode[i].c_str();
        for (unsigned int i = 0; i < fsMergedCode.size(); ++i)
            fsStringsC[i] = fsMergedCode[i].c_str();
        glShaderSource(sVertex, vsMergedCode.size(), vsStringsC, NULL);
        glShaderSource(sFragment, fsMergedCode.size(), fsStringsC, NULL);
        delete[] vsStringsC;
        delete[] fsStringsC;
    }
    else {
        glShaderSource(sVertex, 1, &vertexSource, NULL);
        glShaderSource(sFragment, 1, &fragmentSource, NULL);
    }
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");

    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }

    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, gShader);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::SetFloat(const char *name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const char *name, int value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const char *name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}


void Shader::SetBlockIndex(const char *name, int location, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformBlockBinding(this->ID, glGetUniformBlockIndex(this->ID, name), location);
}

// Explicit uniform location
void Shader::SetFloat(int location, float value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(location, value);
}
void Shader::SetInteger(int location, int value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(location, value);
}
void Shader::SetVector2f(int location, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(location, x, y);
}
void Shader::SetVector2f(int location, const glm::vec2 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(location, value.x, value.y);
}
void Shader::SetVector3f(int location, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(location, x, y, z);
}
void Shader::SetVector3f(int location, const glm::vec3 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(location, value.x, value.y, value.z);
}
void Shader::SetVector4f(int location, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(location, x, y, z, w);
}
void Shader::SetVector4f(int location, const glm::vec4 &value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(int location, const glm::mat4 &matrix, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            GL_ERROR("ERROR::SHADER: Compile-time error: Type: {0} \n {1}", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            GL_ERROR("ERROR::SHADER: Link-time error: Type: {0} \n {1}", type, infoLog);
        }
    }
}