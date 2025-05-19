The binary fetches shaders files using ../shaders/*.frag and ../shaders/*.vert so. So you must be in the build directory to run the executable.

The code is run on Arch Linux x86_64. You may directly use the binaries in cmake-build-* if compatible.

You need an Nvidia GPU.
The required libraries are:
- Vulkan SDK
- GLFW
- GLM
- ImGui
- OpenMP
- NVML (usually installed with the Nvidia driver)

You need to manually install the imgui and update line 11 in CMakeLists.txt to point to the correct location.
Triangle can be compiled by `make` in the triangle directory. The makefile is modified. You can change it if you want to.
All other libraries should be handled by CMake.

There is real type option in the CMakeLists.txt: float or double. If you change it you must also recompile the triangle library. For float type, activate the -DSINGLE switch.