#pragma once

#include <glad/glad.h>

#include "Core/Log.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void printGLInfo() {

    // GL_INFO("OpenGL: GL version: {0}", (const char*)glGetString(GL_VERSION));
    // GL_INFO("OpenGL: GLSL version: {0}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    // GL_INFO("OpenGL: Vendor: {0}", (const char*)glGetString(GL_VENDOR));
    // GL_INFO("OpenGL: Renderer: {0}", (const char*)glGetString(GL_RENDERER));

    ImGui::Text("OpenGL: GL version: %s", (const char*)glGetString(GL_VERSION));
    ImGui::Text("OpenGL: GLSL version: %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("OpenGL: Vendor: %s", (const char*)glGetString(GL_VENDOR));
    ImGui::Text("OpenGL: Renderer: %s", (const char*)glGetString(GL_RENDERER));
}