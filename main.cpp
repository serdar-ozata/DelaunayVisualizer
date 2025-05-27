#include <iostream>
#include <vector>
#include "typedef.h"
#include "plane_sampler.h"
#include "visualizer.h"
#include <algorithm>

#include "omp_quicksort.h"

int main(int argc, char* argv[]) {
    bool no_display = false;
    int num_omp_threads = 1;
    omp_set_nested(1);
    int n = 1000;
    PlaneSamplerData planeSamplerData;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::cout << R"(
Usage: draw [OPTIONS]

Options:

  --threads <n>              Use <n> OpenMP threads (must be ≥ 1).

  --points <n>               Generate <n> points (must be ≥ 1).

  --uniform <x_min> <x_max> <y_min> <y_max>
                             Sample points uniformly from the given rectangular range.

  --gaussian <mu> <sigma>    Sample points from a Gaussian distribution with mean <mu> and std dev <sigma>.

  --circle <r>               Sample points uniformly from a circle of radius <r>.

  --no-display               Run without opening a display window.

Notes:
  - You may specify multiple sampling options (--uniform, --gaussian, --circle),
    but only the *last one given* will be used.
)" << std::endl;
            return EXIT_SUCCESS;
        }

        if (arg == "--no-display") {
            no_display = true;
        }
        else if (arg == "--threads") {
            num_omp_threads = std::stoi(argv[++i]);
            if (num_omp_threads < 1) {
                std::cerr << "Invalid number of threads. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (arg == "--points") {
            n = std::stoi(argv[++i]);
            if (n < 1) {
                std::cerr << "Invalid number of points. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (arg == "--uniform") {
            planeSamplerData.x_min = std::stoi(argv[++i]);
            planeSamplerData.x_max = std::stoi(argv[++i]);
            planeSamplerData.y_min = std::stoi(argv[++i]);
            planeSamplerData.y_max = std::stoi(argv[++i]);
            planeSamplerData.type = 1;
        }
        else if (arg == "--gaussian") {
            planeSamplerData.mu = std::stoi(argv[++i]);
            planeSamplerData.sigma = std::stoi(argv[++i]);
            planeSamplerData.type = 0;
        }
        else if (arg == "--circle") {
            planeSamplerData.r = std::stoi(argv[++i]);
            planeSamplerData.type = 2;
        }
    }
    omp_set_num_threads(num_omp_threads);

    // Example point set

    AppParameters params(n, planeSamplerData);
    if (no_display) {
        return EXIT_SUCCESS;
    }

    TriangulationVisualizer app(params);

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
