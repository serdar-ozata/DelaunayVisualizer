//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_PLANE_SAMPLER_H
#define CS564PROJECT_PLANE_SAMPLER_H
#include "typedef.h"

typedef std::vector<Point_2> plane_t;


plane_t uniform_plane(int n, double x_min, double x_max, double y_min, double y_max);

plane_t gaussian_plane(int n, double sigma, double mu);

plane_t circle_plane(int n, double r);

#endif //CS564PROJECT_PLANE_SAMPLER_H
