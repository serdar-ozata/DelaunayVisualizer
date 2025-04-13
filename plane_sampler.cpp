//
// Created by serdar on 3/21/25.
//

#include "plane_sampler.h"
#include <random>
#include <chrono>

unsigned int get_seed(){
    return std::chrono::system_clock::now().time_since_epoch().count();
}

plane_t gaussian_plane(int n, double sigma, double mu) {
    plane_t points;
    std::default_random_engine generator(get_seed());
    std::normal_distribution<double> x_distribution(mu, sigma);
    std::normal_distribution<double> y_distribution(mu, sigma);
    for (int i = 0; i < n; i++) {
        double x = x_distribution(generator);
        double y = y_distribution(generator);
        points.push_back(Point_2(x, y));
    }
    return points;
}

plane_t uniform_plane(int n, double x_min, double x_max, double y_min, double y_max) {
    plane_t points;
    std::default_random_engine generator(get_seed());
    std::uniform_real_distribution<double> x_distribution(x_min, x_max);
    std::uniform_real_distribution<double> y_distribution(y_min, y_max);
    for (int i = 0; i < n; i++) {
        double x = x_distribution(generator);
        double y = y_distribution(generator);
        points.push_back(Point_2(x, y));
    }
    return points;
}

plane_t circle_plane(int n, double r) {
    plane_t points;
    std::default_random_engine generator(get_seed());
    std::uniform_real_distribution<double> angle_distribution(0, 2 * M_PI);
    for (int i = 0; i < n; i++) {
        double angle = angle_distribution(generator);
        double x = r * cos(angle);
        double y = r * sin(angle);
        points.push_back(Point_2(x, y));
    }
    return points;
}