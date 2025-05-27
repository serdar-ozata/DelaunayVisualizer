The binary fetches shaders files using ../shaders/*.frag and ../shaders/*.vert so. So you must be in the build directory to run the executable.


The required libraries are:
- Vulkan SDK
- GLFW
- GLM
- ImGui (you must define IMGUI_DIR in CMakeLists.txt to the path of the library)
- OpenMP
- NVML (optional, define NVML_LIB_PATH in CMakeLists.txt to the path of the library)

You need to manually install the imgui: https://github.com/ocornut/imgui

There is a "real type" option in the CMakeLists.txt: float or double (default). If you want to change it, you must also recompile the triangle library.
Activate the -DSINGLE switch in the CMakeLists.txt to use float instead of double and run make distclean && make trilibrary inside the triangle directory.

Compile the project with:
mkdir build
cd build
cmake .. -DIMGUI_DIR=path/to/imgui -DNVML_LIB_PATH=path/to/nvml
make
