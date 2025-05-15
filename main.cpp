#include <iostream>
#include <vector>
#include "typedef.h"
#include "plane_sampler.h"
#include "visualizer.h"
#include <algorithm>

#include "omp_quicksort.h"

int main(int argc, char* argv[]) {
    // omp_set_num_threads(1);
    int32_t requested_msaa = 1;
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
            printf("Requested MSAA: %d\n", requested_msaa);
        }
    }
    // Example point set
    // std::vector<Vector2> points = uniform_plane(10'000, -20, 20, -20, 20);
    std::vector<Vector2> points = gaussian_plane(10'000, 600, 0);
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

    TriangulationVisualizer app(params);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
