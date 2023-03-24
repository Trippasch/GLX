-- premake5.lua
workspace "GLX"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "GLX"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "GLXExternal.lua"
