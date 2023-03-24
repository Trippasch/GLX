# GLX Engine
GLX is an early-stage OpenGL rendering engine for learning purposes.

## Getting Started
<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with:
```
git clone --recurse-submodules git@github.com:Trippasch/GLX.git
```
If the repository was cloned non-recursively previously, use ```git submodule update --init``` to clone the necessary submodules.

<ins>**2. Generate Project files:**</ins>

For Windows run the [GenerateProjects.bat](https://github.com/Trippasch/GLX/blob/main/GenerateProjects.bat) file. This will generate the visual studio (2022) solution to compile and run the project.

For Linux run the [GenerateProjects.sh](https://github.com/Trippasch/GLX/blob/main/GenerateProjects.sh) file. This will generate makefiles to compile and run the project. The compiler used inside the script is LLVM's clang but you can change it to gcc as well.

<ins>**3. Clean Project files:**</ins>

For Windows run the [CleanProjects.bat](https://github.com/Trippasch/GLX/blob/main/CleanProjects.bat) file.

For Linux run the [CleanProjects.sh](https://github.com/Trippasch/GLX/blob/main/CleanProjects.sh) file.

## Dependencies
The project uses the following dependencies:
  * [Assimp](https://github.com/assimp/assimp) for 3d model importing.
  * [ImGui](https://github.com/ocornut/imgui) for creating graphical user interfaces (GUIs).
  * [GLM](https://github.com/g-truc/glm) for vector math.
  * [GLFW](https://www.glfw.org/) for creating windows, contexts and surfaces.
  * [Glad](https://glad.dav1d.de/) for generating OpenGL functions.
  * [spdlog](https://github.com/gabime/spdlog) for logging.
  * [stb](https://github.com/nothings/stb) for image loading.
  * [Premake](https://premake.github.io/) for building the project.
