//
// Created by serdar on 4/5/25.
//

#ifndef CS564PROJECT_DELAUNAY_TRIANGULATION_H
#define CS564PROJECT_DELAUNAY_TRIANGULATION_H
#include "typedef.h"

class delaunay_triangulation {
public:
    delaunay_triangulation(const std::vector<sf::Vector2f>& points) : points(points) {
    }
    std::vector<float> medians_y;
    std::vector<std::vector<sf::Vector3f>> chains;
    std::vector<sf::Vector2f> solve() {
        int random_index = rand() % points.size();
        float med_y = points[random_index].y;
        sf::Vector3f* points_3d = new sf::Vector3f[points.size()];
        for (int i = 0; i < points.size(); i++) {
            points_3d[i].x = points[i].x;
            points_3d[i].y = points[i].y;
            points_3d[i].z = points[i].y; // trick to calculate real hull
        }
        int ext_size = 0;
        sf::Vector3f* extreme_points = quick_hull(points_3d, points.size(), true, ext_size);
        std::vector<sf::Vector3f> triangulation = get_triangulation(points_3d, points.size(), extreme_points, ext_size);
        return std::vector<sf::Vector2f>();
    }

    float find_real_median(sf::Vector3f* local_set, size_t size) {
        // Create a vector from the raw pointer
        std::vector<sf::Vector3f> local_set_copy(local_set, local_set + size);
        size_t n = local_set_copy.size();
        auto mid_it = local_set_copy.begin() + n / 2;
        std::nth_element(local_set_copy.begin(), mid_it, local_set_copy.end(), [](const sf::Vector3f& a, const sf::Vector3f& b) {
            return a.y < b.y;
        });
        return mid_it->y;
    }

    std::vector<sf::Vector3f> get_triangulation(sf::Vector3f* local_set, int local_set_size, sf::Vector3f* extreme_points, int extreme_size) {
        if (local_set_size <= 3) {
            std::vector<sf::Vector3f> result;
            for (int i = 0; i < local_set_size; i++) {
                result.push_back(local_set[i]);
            }
            chains.push_back(result); // delete later
            medians_y.push_back(INFINITY); // delete later
            return result;
        }
        float med_y = find_real_median(local_set, local_set_size);
        medians_y.push_back(med_y); // delete later
        // recalculate z
        for (int i = 0; i < local_set_size; i++) {
            local_set[i].z = local_set[i].x * local_set[i].x + (local_set[i].y - med_y) * (local_set[i].y - med_y);
        }
        int hull3f_size = 0;
        sf::Vector3f* hull3f = quick_hull(local_set, local_set_size, true, hull3f_size);
        chains.push_back(std::vector<sf::Vector3f>(hull3f, hull3f + hull3f_size)); // delete later
        // divide the points into two sets
        int left_size = 0;
        std::vector<sf::Vector3f> temp_set;
        int hull_idx = hull3f_size - 1;
        for (int i = 0; i < local_set_size; i++) {
            sf::Vector3f p = local_set[i];
            if (hull_idx >= 0 && p == hull3f[hull_idx]) {
                hull_idx--;
                continue;
            }
            if (p.y < med_y) {
                local_set[left_size++] = p;
            } else {
                temp_set.push_back(p);
            }
        }
        assert(hull_idx == -1);
        std::copy(temp_set.begin(), temp_set.end(), local_set + left_size);
        int right_size = temp_set.size();
        temp_set.clear(); // save memory
        // divide the extreme points into two sets
        int first_ext_left_size = 0, ext_right_size = 0;
        while(first_ext_left_size < extreme_size && extreme_points[first_ext_left_size].y <= med_y) {
            first_ext_left_size++;
        }
        while(first_ext_left_size + ext_right_size < extreme_size && extreme_points[first_ext_left_size + ext_right_size].y > med_y) {
            ext_right_size++;
        }
        int last_ext_left_size = extreme_size - first_ext_left_size - ext_right_size;
        int ext_left_size = first_ext_left_size + ext_right_size;
        sf::Vector3f* last_left_extreme = new sf::Vector3f[last_ext_left_size];
        std::copy(extreme_points + first_ext_left_size + ext_right_size, extreme_points + extreme_size, last_left_extreme);
        std::move(extreme_points + first_ext_left_size, extreme_points + first_ext_left_size + ext_right_size, extreme_points + ext_left_size);
        std::copy(last_left_extreme, last_left_extreme + last_ext_left_size, extreme_points + first_ext_left_size);

        // find the triangulation of the left and right sets
        std::vector<sf::Vector3f> left_del_chain = get_triangulation(local_set, left_size, extreme_points, ext_left_size);
        std::vector<sf::Vector3f> right_del_chain = get_triangulation(local_set + left_size, right_size, extreme_points + ext_left_size, ext_right_size);

        // todo
        return std::vector<sf::Vector3f>();
    }

    sf::Vector3f* quick_hull(sf::Vector3f* lpoints, int size, bool l2_triangulation, int& hull_size) {
        if (size <= 3) {
            hull_size = size;
            return lpoints;
        }
        // assuming points are sorted
        sf::Vector3f left = lpoints[0];
        sf::Vector3f right = lpoints[size - 1];

        std::vector<sf::Vector3f> left_set, right_set;
        for (int i = 1; i < size - 1; i++) {
            sf::Vector3f p = lpoints[i];
            if (p != left && p != right) {
                float side = (right.x - left.x) * (p.z - left.z) - (right.z - left.z) * (p.x - left.x);
                if (side > 0) {
                    left_set.push_back(p);
                } else {
                    right_set.push_back(p);
                }
            }
        }
        std::list<sf::Vector3f> hullL, hullR;
        hullL.push_back(left);
        hullL.push_back(right);
        hullR.push_back(right);
        hullR.push_back(left);
        std::list<sf::Vector3f>::const_iterator leftStartPtr = hullL.begin();
        std::list<sf::Vector3f>::const_iterator rightStartPtr = hullR.begin();

        sf::Vector3f* hull;
        if (l2_triangulation) {
            find_hull(rightStartPtr, right_set, hullR); // the right set is empty if it's lifted
            hull = new sf::Vector3f[hullR.size()];
            hull_size = hullR.size();
            std::copy(hullR.begin(), hullR.end(), hull);
        } else {
            find_hull(leftStartPtr, left_set, hullL);
            find_hull(rightStartPtr, right_set, hullR);
            hull = new sf::Vector3f[hullL.size() + hullR.size() - 2];
            hull_size = hullL.size() + hullR.size() - 2;

            hullL.pop_back();
            hullR.pop_back();

            std::copy(hullL.begin(), hullL.end(), hull);
            std::copy(hullR.begin(), hullR.end(), hull + hullL.size() - 1);
        }
        return hull;
    }
    const std::vector<sf::Vector2f>& points;
private:

    void find_hull(std::list<sf::Vector3f>::const_iterator leftIt, const std::vector<sf::Vector3f>& local_points, std::list<sf::Vector3f>& hull) {
        if (local_points.empty()) {
            return;
        }
        std::list<sf::Vector3f>::const_iterator rightIt = std::next(leftIt);
        sf::Vector3f left = *leftIt;
        sf::Vector3f right = *rightIt;

        float max_dist = 0;
        sf::Vector3f farthest_point;
        for (const auto& p : local_points) {
            float dist = line_distance(left, right, p);
            if (dist > max_dist) {
                max_dist = dist;
                farthest_point = p;
            }
        }
        hull.insert(rightIt, farthest_point);

        std::vector<sf::Vector3f> left_set, right_set;

        for (const auto& p : local_points) {
            if (p != left && p != right && p != farthest_point) {
                float sideLeft = (farthest_point.x - left.x) * (p.z - left.z) - (farthest_point.z - left.z) * (p.x - left.x);
                float sideRight = (right.x - farthest_point.x) * (p.z - farthest_point.z) - (right.z - farthest_point.z) * (p.x - farthest_point.x);
                if (sideLeft > 0) {
                    left_set.push_back(p);
                } else if (sideRight > 0) {
                    right_set.push_back(p);
                }
            }
        }

        std::list<sf::Vector3f>::const_iterator farthestIt = std::next(leftIt);
        find_hull(leftIt, left_set, hull);
        find_hull(farthestIt, right_set, hull);
    }

    inline float line_distance(const sf::Vector3f& a, const sf::Vector3f& b, const sf::Vector3f& p) {
        float num = std::abs((b.z - a.z) * p.x - (b.x - a.x) * p.z + b.x * a.z - b.z * a.x);
        float den = std::sqrt((b.z - a.z) * (b.z - a.z) + (b.x - a.x) * (b.x - a.x));
        return num / den;
    }



    std::vector<sf::Vector2f> merge(const std::vector<sf::Vector2f>& left, const std::vector<sf::Vector2f>& right) {
        //todo
        return std::vector<sf::Vector2f>();
    }
};


#endif //CS564PROJECT_DELAUNAY_TRIANGULATION_H
