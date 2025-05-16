#include <iostream>
#include <vector>
#include "typedef.h"
#include "plane_sampler.h"
#include "visualizer.h"
#include <algorithm>

#include "omp_quicksort.h"

int main(int argc, char* argv[]) {
    int32_t requested_msaa = 1;
    bool no_display = false;
    int num_omp_threads = 1;
    int n = 1000;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--msaa") {
            requested_msaa = std::stoi(argv[++i]);
            if (requested_msaa < 1 || requested_msaa > 64) {
                std::cerr << "Invalid MSAA value. Must be between 1 and 64." << std::endl;
                return EXIT_FAILURE;
            }
            if (log2(requested_msaa) != static_cast<int>(log2(requested_msaa))) {
                std::cerr << "MSAA value must be a power of 2." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (std::string(argv[i]) == "--no-display") {
            no_display = true;
        } else if (std::string(argv[i]) == "--threads") {
            num_omp_threads = std::stoi(argv[++i]);
            if (num_omp_threads < 1) {
                std::cerr << "Invalid number of threads. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (std::string(argv[i]) == "--points") {
            n = std::stoi(argv[++i]);
            if (n < 1) {
                std::cerr << "Invalid number of points. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        }
    }
    omp_set_num_threads(num_omp_threads);

    // Example point set
    // std::vector<Vector2> points = uniform_plane(10'000, -20, 20, -20, 20);
    std::vector<Vector2> points = gaussian_plane(n, 100, 0);
    // std::vector<Point_2> points = circle_plane(7, 10);
    // Compute Delaunay triangulation
    // std::vector<Point_2> points{Point_2{-2, 0}, Point_2{-1.5, -1}, Point_2{-1, -0.5}, Point_2{1, -0.5},
    //     Point_2{2, 0}, Point_2{0, 2}, Point_2{0, -2}};
    char text[100];
    sprintf(text, "Delaunay Triangulation with %d points", (int)points.size());
    run_parallel_quicksort(points.data(), points.size());

    for (int i = 0; i < points.size() - 1; i++) {
        if (points[i].x == points[i + 1].x) {
            std::cerr << "Duplicate points detected. Exiting." << std::endl;
            return EXIT_FAILURE;
        }
    }

    VkAppParameters params(points);
    params.msaa_bits = requested_msaa;
    if (no_display) {
        return EXIT_SUCCESS;
    }

    TriangulationVisualizer app(params);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
