//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_PLANE_SAMPLER_H
#define CS564PROJECT_PLANE_SAMPLER_H
#include "typedef.h"
#include <vector>

typedef std::vector<Vector2> plane_t;


typedef struct {
    int x_min, x_max, y_min, y_max;
    int sigma = 500, mu = 0;
    int r;
    int type = 0; // 0: gaussian, 1: uniform, 2: circle
} PlaneSamplerData;


plane_t uniform_plane(int n, double x_min, double x_max, double y_min, double y_max);

plane_t gaussian_plane(int n, double sigma, double mu);

plane_t circle_plane(int n, double r);

#endif //CS564PROJECT_PLANE_SAMPLER_H
