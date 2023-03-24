-- GLXExternal.lua

IncludeDir = {}
IncludeDir["glm"] = "../vendor/glm"
IncludeDir["Glad"] = "../vendor/Glad/include"

group "Dependencies"
    include "vendor/ImGui"
    include "vendor/Glad"
    include "vendor/GLFW"
    include "vendor/Assimp"
group ""

include "GLX"
