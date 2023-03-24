#pragma once

#include <glad/glad.h>

#include "Core/Log.h"

void printGLInfo() {

    GL_INFO("OpenGL: GL version: {0}", (const char*)glGetString(GL_VERSION));
    GL_INFO("OpenGL: GLSL version: {0}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    GL_INFO("OpenGL: Vendor: {0}", (const char*)glGetString(GL_VENDOR));
    GL_INFO("OpenGL: Renderer: {0}", (const char*)glGetString(GL_RENDERER));
}