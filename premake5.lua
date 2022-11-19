-- premake5.lua
workspace "OpenGL"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "OpenGL"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "OpenGLExternal.lua"
