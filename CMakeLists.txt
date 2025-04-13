cmake_minimum_required(VERSION 3.30)
project(cs564project)

set(CMAKE_CXX_STANDARD 20)

# Find CGAL package
find_package(CGAL REQUIRED)

# Find SFML package
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)


# Add executable
add_executable(cs564project main.cpp
        plane_sampler.cpp
        plane_sampler.h
        typedef.h
        visualizer.cpp
        visualizer.h
        delaunay_triangulation.h
        typedef.cpp
)

# Link CGAL and SFML libraries
target_link_libraries(cs564project CGAL::CGAL sfml-graphics sfml-window sfml-system)