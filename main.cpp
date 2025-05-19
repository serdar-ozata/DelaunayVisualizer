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
    omp_set_nested(1);
    int n = 1000;
    PlaneSamplerData planeSamplerData;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::cout << R"(
Usage: draw [OPTIONS]

Options:
  --msaa <samples>           Enable MSAA with the given sample count.
                             Must be a power of 2 between 1 and 64. (broken for now)

  --no-display               Run without opening a display window. (broken for now)

  --threads <n>              Use <n> OpenMP threads (must be ≥ 1).

  --points <n>               Generate <n> points (must be ≥ 1).

  --uniform <x_min> <x_max> <y_min> <y_max>
                             Sample points uniformly from the given rectangular range.

  --gaussian <mu> <sigma>    Sample points from a Gaussian distribution with mean <mu> and std dev <sigma>.

  --circle <r>               Sample points uniformly from a circle of radius <r>.

Notes:
  - You may specify multiple sampling options (--uniform, --gaussian, --circle),
    but only the *last one given* will be used.
)" << std::endl;
            return EXIT_SUCCESS;
        }

        if (arg == "--msaa") {
            requested_msaa = std::stoi(argv[++i]);
            if (requested_msaa < 1 || requested_msaa > 64) {
                std::cerr << "Invalid MSAA value. Must be between 1 and 64." << std::endl;
                return EXIT_FAILURE;
            }
            if (log2(requested_msaa) != static_cast<int>(log2(requested_msaa))) {
                std::cerr << "MSAA value must be a power of 2." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--no-display") {
            no_display = true;
        } else if (arg == "--threads") {
            num_omp_threads = std::stoi(argv[++i]);
            if (num_omp_threads < 1) {
                std::cerr << "Invalid number of threads. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--points") {
            n = std::stoi(argv[++i]);
            if (n < 1) {
                std::cerr << "Invalid number of points. Must be greater than 0." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--uniform") {
            planeSamplerData.x_min = std::stoi(argv[++i]);
            planeSamplerData.x_max = std::stoi(argv[++i]);
            planeSamplerData.y_min = std::stoi(argv[++i]);
            planeSamplerData.y_max = std::stoi(argv[++i]);
            planeSamplerData.type = 1;
        } else if (arg == "--gaussian") {
            planeSamplerData.mu = std::stoi(argv[++i]);
            planeSamplerData.sigma = std::stoi(argv[++i]);
            planeSamplerData.type = 0;
        } else if (arg == "--circle") {
            planeSamplerData.r = std::stoi(argv[++i]);
            planeSamplerData.type = 2;
        }
    }
    omp_set_num_threads(num_omp_threads);

    // Example point set

    VkAppParameters params(n);
    params.planeSamplerData = planeSamplerData;
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
