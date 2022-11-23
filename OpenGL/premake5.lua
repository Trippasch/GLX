project "OpenGL"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    staticruntime "on"
    debugdir "%{wks.location}"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src",
        "../vendor/ImGui",
        "../vendor/spdlog/include",
        "../vendor/stb",
        "../vendor/GLFW/include",
        "../vendor/Assimp/include",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}"
    }

    defines {
        "GLFW_INCLUDE_NONE"
    }

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "GL_PLATFORM_WINDOWS" }

        links {
            "ImGui",
            "GLFW",
            "Assimp",
            "Glad",
            "opengl32.lib"
        }

    filter "system:linux"
        systemversion "latest"

        links {
            "ImGui",
            "GLFW",
            "Assimp",
            "Glad",
            "GL",
        }

    filter "configurations:Debug"
        defines { "GL_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "GL_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "GL_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
