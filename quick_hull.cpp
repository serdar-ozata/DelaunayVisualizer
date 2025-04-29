//
// Created by serdar on 4/25/25.
//

#include "quick_hull.h"
#include <math.h>
#define CALCULATE_SIDE(left, right, p) \
(((right.x) - (left.x)) * ((p.z) - (left.z)) - ((right.z) - (left.z)) * ((p.x) - (left.x)))

std::array<std::list<my::Point>, 2> quick_hull(const my::Point* lpoints, const int size) {
    std::list<my::Point> hullR, hullL;
    if (size == 3) {
        hullL.push_back(lpoints[0]);
        hullL.push_back(lpoints[1]);
        hullL.push_back(lpoints[2]);
        hullR.push_back(lpoints[0]);
        hullR.push_back(lpoints[2]);
        return std::array{hullL, hullR};
    }
    // assuming points are sorted
    my::Point left = lpoints[0];
    my::Point right = lpoints[size - 1];

    std::vector<my::Point> left_set, right_set;
    for (int i = 1; i < size - 1; i++) {
        my::Point p = lpoints[i];
        if (const float side = CALCULATE_SIDE(left, right, p); side > 0)
        {
            right_set.push_back(p);
        }
        else
        {
            left_set.push_back(p);
        }
    }

    left.onEdge = true;
    right.onEdge = true;

    hullL.push_back(left);
    hullL.push_back(right);
    hullR.push_back(left);
    hullR.push_back(right);

    const std::list<my::Point>::const_iterator rightStartPtr = hullR.begin();
    const std::list<my::Point>::const_iterator leftStartPtr = hullL.begin();
    find_hull(leftStartPtr, left_set, hullL, false);
    find_hull(rightStartPtr, right_set, hullR, true);

    return std::array{hullL, hullR};
}

std::list<my::Point> lifted_quick_hull(my::Point* lpoints, int size) {
    std::list<my::Point> hull;
    if (size == 3) {
        hull.push_back(lpoints[0]);
        hull.push_back(lpoints[1]);
        hull.push_back(lpoints[2]);
        return hull;
    }
    // assuming points are sorted
    const my::Point left = lpoints[0];
    const my::Point right = lpoints[size - 1];

    std::vector<my::Point> left_set, right_set;
    for (int i = 1; i < size - 1; i++) {
        my::Point p = lpoints[i];
        if (const float side = CALCULATE_SIDE(left, right, p); side <= 0) {
            right_set.push_back(p);
        }
    }
    hull.push_back(left);
    hull.push_back(right);

    hull.front().onEdge = true;
    hull.back().onEdge = true;

    const std::list<my::Point>::const_iterator rightStartPtr = hull.begin();

    find_hull(rightStartPtr, right_set, hull, false); // the right set is empty if it's lifted

    return hull;
}

void find_hull(const std::list<my::Point>::const_iterator leftIt, const std::vector<my::Point>& local_points,
    std::list<my::Point>& hull, const bool counter_clockwise) {
    if (local_points.empty()) {
        return;
    }
    const std::list<my::Point>::const_iterator rightIt = std::next(leftIt);
    const my::Point left = *leftIt;
    const my::Point right = *rightIt;

    float max_dist = 0;
    my::Point farthest_point;
    for (const auto& p : local_points) {
        const float dist = line_distance(left, right, p);
        if (dist > max_dist) {
            max_dist = dist;
            farthest_point = p;
        }
    }
    farthest_point.onEdge = true;
    hull.insert(rightIt, farthest_point);

    std::vector<my::Point> left_set, right_set;

    for (const auto& p : local_points) {
        // float sideLeft = (farthest_point.x - left.x) * (p.z - left.z) - (farthest_point.z - left.z) * (p.x - left.x);
        // float sideRight = (right.x - farthest_point.x) * (p.z - farthest_point.z) - (right.z - farthest_point.z) * (p.x - farthest_point.x);

        const float sideLeft = counter_clockwise ? CALCULATE_SIDE(left, farthest_point, p) :
            CALCULATE_SIDE(farthest_point, left, p);
        const float sideRight =  counter_clockwise ? CALCULATE_SIDE(farthest_point, right, p) :
            CALCULATE_SIDE(right, farthest_point, p);
        if (sideLeft > 0) {
            left_set.push_back(p);
        } else if (sideRight > 0) {
            right_set.push_back(p);
        }
    }

    const std::list<my::Point>::const_iterator farthestIt = std::next(leftIt);
    find_hull(leftIt, left_set, hull, counter_clockwise);
    find_hull(farthestIt, right_set, hull, counter_clockwise);
}

inline float line_distance(const my::Point& a, const my::Point& b, const my::Point& p) {
    float num = std::abs((b.z - a.z) * p.x - (b.x - a.x) * p.z + b.x * a.z - b.z * a.x);
    float den = std::sqrt((b.z - a.z) * (b.z - a.z) + (b.x - a.x) * (b.x - a.x));
    return num / den;
}
