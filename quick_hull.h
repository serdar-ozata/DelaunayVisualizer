//
// Created by serdar on 4/25/25.
//

#ifndef QUICK_HULL_H
#define QUICK_HULL_H
#include "typedef.h"
#include <list>

std::array<std::list<my::Point>, 2> quick_hull(const my::Point* lpoints, int size);

std::list<my::Point> lifted_quick_hull(my::Point* lpoints, int size);

void find_hull(std::list<my::Point>::const_iterator leftIt, const std::vector<my::Point>& local_points, std::list<my::Point>& hull, const bool counter_clockwise);

inline float line_distance(const my::Point& a, const my::Point& b, const my::Point& p);

#endif //QUICK_HULL_H
