# GLX Engine
GLX is an early-stage cross-platform OpenGL rendering engine for learning purposes.

![GLX](https://i.imgur.com/qcN1wqx.png)

## Getting Started
<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with:
```
git clone --recurse-submodules git@github.com:Trippasch/GLX.git
```
You can also use ```--remote-submodules``` to clone the submodules to their latest revision.
If the repository was cloned non-recursively previously, use ```git submodule update --init``` to clone the necessary submodules.

<ins>**2. Generate Project files:**</ins>

You can use [CMake](https://cmake.org/) to generate the project files and build the project.

To streamline the building process you can also use the [invoke](https://www.pyinvoke.org/) python library. You can install it with ```pip install invoke```.

* ```inv config``` to generate the project files.
* ```inv build``` to build the project.
* ```inv run``` to run the project.

To use Debug mode do:
* ```inv config --build-type=Debug``` to generate the project files.
* ```inv build --build-type=Debug``` to build the project.
* ```inv run --build-type=Debug``` to run the project.

** You can change the configuration of cmake in [tasks.py](https://github.com/Trippasch/GLX/blob/main/tasks.py).

<ins>**3. Clean Project files:**</ins>
* ```inv clean``` to clean the project files.

## Dependencies
The project uses the following dependencies:
  * [Assimp](https://github.com/assimp/assimp) for 3d model importing.
  * [ImGui](https://github.com/ocornut/imgui) for creating graphical user interfaces (GUIs).
  * [GLM](https://github.com/g-truc/glm) for vector math.
  * [GLFW](https://www.glfw.org/) for creating windows, contexts and surfaces.
  * [Glad](https://glad.dav1d.de/) for generating OpenGL functions.
  * [spdlog](https://github.com/gabime/spdlog) for logging.
  * [stb](https://github.com/nothings/stb) for image loading.
  * [CMake](https://cmake.org/) for building the project.
