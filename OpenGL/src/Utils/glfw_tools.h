#pragma once

#include <GLFW/glfw3.h>
#include <string>

#include "Core/Log.h"

void printGLFWInfo(GLFWwindow* w){
    int p = glfwGetWindowAttrib(w, GLFW_OPENGL_PROFILE);
    std::string version = glfwGetVersionString();
    std::string opengl_profile = "";
    if(p == GLFW_OPENGL_COMPAT_PROFILE){
        opengl_profile = "OpenGL Compatibility Profile";
    }
    else if (p == GLFW_OPENGL_CORE_PROFILE){
        opengl_profile = "OpenGL Core Profile";
    }
    GL_INFO("GLFW: {0}", version.c_str());
    GL_INFO("GLFW: {0} {1}", opengl_profile.c_str(), p);
}
