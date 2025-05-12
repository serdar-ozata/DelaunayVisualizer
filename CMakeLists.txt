cmake_minimum_required(VERSION 3.30)
project(cs564project)

set(CMAKE_CXX_STANDARD 20)

# Find SFML package
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# Add triangle.o as an imported object library
add_library(triangle OBJECT IMPORTED)
set_target_properties(triangle PROPERTIES
        IMPORTED_OBJECTS "${CMAKE_CURRENT_SOURCE_DIR}/triangle/triangle.o"
)

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
)

# Include triangle's header directory
target_include_directories(cs564project PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/triangle
)

# Link triangle.o and SFML
target_link_libraries(cs564project
        triangle
        sfml-graphics
        sfml-window
        sfml-system
)

# Optional compile flags
target_compile_options(cs564project PRIVATE -frounding-math -ffast-math)
