@echo off

del /S /Q OpenGL.sln
rmdir /S /Q "bin"
rmdir /S /Q "bin-int"
del /S /Q "OpenGL\OpenGL.vcxproj"
del /S /Q "OpenGL\OpenGL.vcxproj.user"
del /S /Q "OpenGL\OpenGL.vcxproj.filters"

del /S /Q "vendor\ImGui\ImGui.vcxproj"
rmdir /S /Q "vendor\ImGui\bin"
rmdir /S /Q "vendor\ImGui\bin-int"

del /S /Q "vendor\Glad\Glad.vcxproj"
del /S /Q "vendor\Glad\Glad.vcxproj.filters"
rmdir /S /Q "vendor\Glad\bin"
rmdir /S /Q "vendor\Glad\bin-int"

del /S /Q "vendor\Assimp\assimp.vcxproj"
del /S /Q "vendor\Assimp\assimp.vcxproj.filters"
rmdir /S /Q "vendor\Assimp\bin"
rmdir /S /Q "vendor\Assimp\bin-int"

del /S /Q "vendor\GLFW\glfw.vcxproj"
del /S /Q "vendor\GLFW\glfw.vcxproj.filters"
rmdir /S /Q "vendor\GLFW\bin"
rmdir /S /Q "vendor\GLFW\bin-int"

pause
