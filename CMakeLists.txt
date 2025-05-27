cmake_minimum_required(VERSION 3.30)
project(cs564project)

set(CMAKE_CXX_STANDARD 20)

# vulkan, glfw, openmp
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenMP REQUIRED)
if(NVML_LIB_PATH)
    find_library(NVML_LIBRARY
            NAMES nvidia-ml nvml
            PATHS ${NVML_LIB_PATH}
            NO_DEFAULT_PATH
            REQUIRED
    )
else ()
    message(STATUS "NVML_LIB_PATH not set, NVML library will not be linked.")
endif ()

if(NOT DEFINED IMGUI_DIR)
    message(FATAL_ERROR "IMGUI_DIR is not defined. Please set it with -DIMGUI_DIR=/path/to/imgui")
endif()

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)
include_directories(${IMGUI_DIR})

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/shaders/*.frag"
        "${CMAKE_SOURCE_DIR}/shaders/*.vert"
)

set(SPIRV_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/shaders")
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv")

    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND glslc ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            COMMENT "Compiling ${GLSL} to ${SPIRV}"
            VERBATIM
    )
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_BINARIES})


# Add executable
add_executable(cs564project
        main.cpp
        plane_sampler.cpp
        plane_sampler.h
        typedef.h
        visualizer.cpp
        visualizer.h
        delaunay_triangulation.h
        typedef.cpp
        quick_hull.cpp
        quick_hull.h
        GpuStats.h
        ${IMGUI_SOURCES}
        omp_quicksort.cpp
        omp_quicksort.h
)
add_dependencies(cs564project compile_shaders)

# Add triangle.o as an imported object library
add_library(triangle OBJECT IMPORTED)
set_target_properties(triangle PROPERTIES
        IMPORTED_OBJECTS "${CMAKE_CURRENT_SOURCE_DIR}/triangle/triangle.o"
)


# Include triangle's header directory
target_include_directories(cs564project PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/triangle
        ${Vulkan_INCLUDE_DIR}
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

# Link triangle.o and SFML
target_link_libraries(cs564project
        triangle
        Vulkan::Vulkan
        glfw
        OpenMP::OpenMP_CXX
)
if (NVML_LIBRARY)
    target_link_libraries(cs564project ${NVML_LIBRARY})
    target_compile_definitions(cs564project PRIVATE ENABLE_GPUSTATS)
endif ()

# Optional compile flags
target_compile_options(cs564project PRIVATE -frounding-math -ffast-math -fopenmp -Wno-write-strings)
target_compile_definitions(cs564project PRIVATE DOUBLE_REAL)
