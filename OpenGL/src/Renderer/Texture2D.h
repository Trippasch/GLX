#pragma once

#include <glad/glad.h>

#include <string>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
    // holds the ID of the texture object, used for all texture operations to reference to this particlar texture
    GLuint ID;
    // texture image dimensions
    GLuint Width, Height; // width and height of loaded image in pixels
    // texture Format
    GLuint Internal_Format; // format of texture object
    GLuint Image_Format;    // format of loaded image
    // texture configuration
    GLuint Wrap_S;     // wrapping mode on S axis
    GLuint Wrap_T;     // wrapping mode on T axis
    GLuint Filter_Min; // filtering mode if texture pixels < screen pixels
    GLuint Filter_Max; // filtering mode if texture pixels > screen pixels
    GLuint Data_Type;

    std::string Type;
    std::string Path;

    // constructor (sets default texture modes)
    Texture2D();
    // generates texture from image data
    void Generate(GLuint width, GLuint height, const void *data, GLboolean mipmap);
    // binds the texture as the current active GL_TEXTURE_2D texture object
    void Bind(GLuint index) const;
    void Destroy() const;
    static void UnBind();
    static void UnBindCubemap();
};