//
// Created by serdar on 4/5/25.
//

#ifndef CS564PROJECT_DELAUNAY_TRIANGULATION_H
#define CS564PROJECT_DELAUNAY_TRIANGULATION_H
#include "typedef.h"
#include "quick_hull.h"
#include <math.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
// #include <omp.h>
#include <iostream>
#include <omp.h>

extern "C" {
#include "triangle/triangle.h"
}

#include <list>

#define HANDLE_EDGE_LIST \
{ \
    auto& current_chain = get_chain_output(); \
    current_chain.push_back(std::vector<int>()); \
    current_chain.back().reserve(nonoverlapping_count + 2); \
    auto tmp_mid_it = mid_it; \
    while (0 < nonoverlapping_count) { \
        current_chain.back().push_back(tmp_mid_it->getId()); \
        tmp_mid_it--; \
        nonoverlapping_count--; \
    } \
    current_chain.back().push_back(tmp_mid_it->getId());\
    tmp_mid_it--; \
    current_chain.back().push_back(tmp_mid_it->getId()); \
}

#define HANDLE_OVERLAPPING_BOTTOM \
lower_chain.erase(lower_chain.begin(), overlapping_it); \
lower_ref_it = std::prev(mid_it);

#define HANDLE_OVERLAPPING_TOP \
upper_chain.erase(upper_chain.begin(), overlapping_it); \
upper_ref_it = std::prev(mid_it);

class delaunay_triangulation {
public:
    delaunay_triangulation(const std::vector<Vector2>& points) : points(points) {
    }

    const std::vector<Vector2>& points;
    std::vector<REAL> medians_y;
    std::vector<std::vector<int>>* chains = new std::vector<std::vector<int>>[omp_get_max_threads()];

    void solve() {
        my::Point* points_3d = new my::Point[points.size()];
        if (points.size() >= 1 << 31) {
            std::cerr << "There are too many points! Integer overflow may occur." << std::endl;
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < points.size(); i++) {
            points_3d[i].x = points[i].x;
            points_3d[i].y = points[i].y;
            points_3d[i].z = points[i].y;
            points_3d[i].id = i;
        }
        // auto quick_hull_start = omp_get_wtime();
        std::list<my::Point> lower_chain, upper_chain;
#pragma omp parallel
        {
#pragma omp single
            {
                auto p = quick_hull(points_3d, points.size());
                lower_chain = p[0];
                upper_chain = p[1];
            }
        }
        auto& current_chain = chains[0];
        current_chain.push_back(std::vector<int>());
        auto& added_vec = current_chain.back();
        added_vec.reserve(upper_chain.size() + lower_chain.size());
        // add counterclockwise
        for (const auto& p : lower_chain) {
            added_vec.push_back(p.getId());
        }
        for (auto it = std::next(upper_chain.rbegin()); it != upper_chain.rend(); ++it) {
            added_vec.push_back(it->getId());
        }

        // update onEdge
        auto upper_it = upper_chain.begin();
        auto lower_it = lower_chain.begin();
        for (int i = 0; i < points.size(); i++) {
            my::Point& p = points_3d[i];
            if (*upper_it == p) {
                p.setOnEdge();
                ++upper_it;
            }
            if (*lower_it == p) {
                p.setOnEdge();
                ++lower_it;
            }
        }
#pragma omp parallel
        {
#pragma omp single
            {
                get_triangulation(points_3d, points.size(), upper_chain, lower_chain);
            }
        }
        std::cout << std::endl;
    }

    static REAL find_real_median(my::Point* local_set, size_t size) {
        // Create a vector from the raw pointer
        const auto mid_it = local_set + size / 2;
        std::nth_element(local_set, mid_it, local_set + size, [](const my::Point& a, const my::Point& b) {
            return a.y < b.y;
        });
        return mid_it->y;
    }

    enum MiddleChainState {
        OVERLAPPING_NEITHER = 0,
        OVERLAPPING_TOP = 1,
        OVERLAPPING_BOTTOM = 2,
    };

    void triangulate_polygon(const std::list<my::Point>& upper_chain, const std::list<my::Point>& lower_chain) {
        const int size = upper_chain.size() + lower_chain.size() - 2;
        if (size == 4) {
            if (upper_chain.size() == 3) {
                const my::Point& p1 = upper_chain.front();
                const my::Point& p2 = *std::next(lower_chain.begin());
                const my::Point& p3 = upper_chain.back();
                const my::Point& p4 = *std::next(upper_chain.begin());
                const Circle c = circumcircle(p1, p4, p2);
                if (isInside(c, p3)) {
                    get_chain_output().push_back(std::vector{p1.getId(), p3.getId()});
                }
                else {
                    get_chain_output().push_back(std::vector{p2.getId(), p4.getId()});
                }
            }
            else {
                const std::list<my::Point>& long_chain = upper_chain.size() > lower_chain.size()
                                                             ? upper_chain
                                                             : lower_chain;
                const std::list<my::Point>& short_chain = upper_chain.size() > lower_chain.size()
                                                              ? lower_chain
                                                              : upper_chain;
                const my::Point& p1 = short_chain.front();
                const my::Point& p2 = *std::next(long_chain.rbegin());
                const my::Point& p3 = short_chain.back();
                const Circle c = circumcircle(p1, p2, p3);
                const my::Point& p4 = *std::next(long_chain.begin());
                if (isInside(c, p4)) {
                    get_chain_output().push_back(std::vector{p3.getId(), p4.getId()});
                }
                else {
                    get_chain_output().push_back(std::vector{p1.getId(), p2.getId()});
                }
            }
        }
        else if (size > 4) {
            // const std::list<my::Point>& long_chain = upper_chain.size() > lower_chain.size() ? upper_chain : lower_chain;
            // const std::list<my::Point>& short_chain = upper_chain.size() > lower_chain.size() ? lower_chain : upper_chain;
            // const std::list<my::Point>& selected_chain = short_chain.size() > 2 ? short_chain : long_chain;
            // auto it = selected_chain.begin();
            // for (int i = 0; i < selected_chain.size() / 2; i++) {
            //     ++it;
            // }
            // const my::Point& p1 = *it;
            // const my::Point& prev = *std::prev(it);
            // const my::Point& next = *std::next(it);
            triangulateio in, out;
            in.numberofpoints = size;
            in.numberofpointattributes = 1;
            in.pointattributelist = new REAL[size];
            in.pointmarkerlist = nullptr;
            in.numberofholes = 0;
            in.numberofregions = 0;
            in.regionlist = nullptr;
            in.pointlist = new REAL[size * 2];
            int idx = 0;
            for (const auto& p : upper_chain) {
                in.pointattributelist[idx / 2] = p.getId();
                in.pointlist[idx++] = p.x;
                in.pointlist[idx++] = p.y;
            }
            for (auto it = std::next(lower_chain.rbegin()); it != std::prev(lower_chain.rend()); ++it) {
                in.pointattributelist[idx / 2] = it->getId();
                in.pointlist[idx++] = it->x;
                in.pointlist[idx++] = it->y;
            }
            in.numberofsegments = size;
            in.segmentlist = new int[size * 2];
            in.segmentmarkerlist = nullptr;
            for (int i = 0; i < size; i++) {
                in.segmentlist[i * 2] = i;
                in.segmentlist[i * 2 + 1] = (i + 1) % size;
            }
            out.pointlist = nullptr;
            out.pointattributelist = nullptr;
            out.pointmarkerlist = nullptr;
            out.trianglelist = nullptr;
            out.triangleattributelist = nullptr;
            out.trianglearealist = nullptr;
            out.neighborlist = nullptr;
            out.numberoftriangles = 0;
            out.numberofcorners = 0;
            out.numberoftriangleattributes = 0;
            out.segmentlist = nullptr;
            out.segmentmarkerlist = nullptr;
            out.numberofsegments = 0;
            out.edgelist = nullptr;
            out.normlist = nullptr;
            out.edgemarkerlist = nullptr;
            out.numberofedges = 0;

            triangulate("pNzqeEQ", &in, &out, nullptr);
            for (int i = 0; i < out.numberofedges; i++) {
                const int first_idx = out.edgelist[i * 2];
                const int second_idx = out.edgelist[i * 2 + 1];
                if ((second_idx + 1) % size == first_idx) {
                    continue;
                }
                const int first_id = in.pointattributelist[first_idx];
                const int second_id = in.pointattributelist[second_idx];
                auto& current_chain = get_chain_output();
                current_chain.push_back(std::vector<int>{first_id, second_id});

                // chains.push_back(std::vector{my::Point(in.pointlist[first_idx * 2], in.pointlist[first_idx * 2 + 1]),
                //                   my::Point(in.pointlist[second_idx * 2], in.pointlist[second_idx * 2 + 1])});
            }
            delete[] in.pointlist;
            delete[] in.segmentlist;
            delete[] in.pointattributelist;
        }
    }

    void get_triangulation(my::Point* local_set, const int local_set_size, std::list<my::Point>& upper_chain,
                           std::list<my::Point>& lower_chain) {
        if (local_set_size - upper_chain.size() - lower_chain.size() == -2) {
            return triangulate_polygon(upper_chain, lower_chain);
        }
        Vector2 median;
        auto upper_it = upper_chain.begin();
        auto lower_it = lower_chain.begin();

        const int free_points = local_set_size - upper_chain.size() - lower_chain.size() + 2;
        int median_idx = free_points / 2;
        for (int i = 0; i < local_set_size; i++) {
            my::Point& p = local_set[i];
            if (median_idx >= 0 && !p.onEdge()) {
                median_idx--;
                if (median_idx <= 0) {
                    median = Vector2(p.x, p.y);
                    break;
                }
            }
        }
        //medians_y.push_back(median.y); // delete later
        // recalculate z
        for (int i = 0; i < local_set_size; i++) {
            local_set[i].z = (local_set[i].x - median.x) * (local_set[i].x - median.x) + (local_set[i].y - median.y) * (
                local_set[i].y - median.y);
        }
        std::list<my::Point> middle_chain = lifted_quick_hull(local_set, local_set_size);
        assert(std::is_sorted(middle_chain.begin(), middle_chain.end()));
        assert(std::is_sorted(upper_chain.begin(), upper_chain.end()));
        assert(std::is_sorted(lower_chain.begin(), lower_chain.end()));
        assert(std::is_sorted(local_set, local_set + local_set_size));
        assert(*middle_chain.begin() == *upper_chain.begin());
        assert(*std::prev(middle_chain.end()) == *std::prev(lower_chain.end()));

        // divide the points into two sets
        std::vector<my::Point> top_set, bottom_set;
        auto chain_point_it = middle_chain.begin();

        for (int i = 0; i < local_set_size; i++) {
            my::Point& p = local_set[i];
            if (chain_point_it != middle_chain.end() && *chain_point_it == p) {
                p.setOnEdge();
                chain_point_it = std::next(chain_point_it);
            }
            if (!p.onEdge()) {
                my::Point& prev_chain_point = *std::prev(chain_point_it);
                if (p.y < median.y) {
                    bottom_set.push_back(p);
                }
                else {
                    top_set.push_back(p);
                }
            }
        }
        // END DIVIDE POINTS
        // auto& current_chain = get_chain_output();
        // current_chain.push_back(std::vector<int>());
        // auto& added_vec = current_chain.back();
        // added_vec.reserve(middle_chain.size());
        // for (const auto& p : middle_chain) {
        //     added_vec.push_back(p.getId());
        // }

        auto mid_it = std::next(middle_chain.begin());
        upper_it = std::next(upper_chain.begin());
        lower_it = std::next(lower_chain.begin());
        auto upper_ref_it = middle_chain.begin();
        auto lower_ref_it = middle_chain.begin();
        MiddleChainState state = OVERLAPPING_NEITHER;
        std::list<my::Point>::const_iterator overlapping_it; // used by both chains
        if (*mid_it == *upper_it) {
            state = OVERLAPPING_TOP;
            overlapping_it = upper_it;
        }
        else if (*mid_it == *lower_it) {
            state = OVERLAPPING_BOTTOM;
            overlapping_it = lower_it;
        }
        std::vector<std::array<std::list<my::Point>, 2>> upper_chain_pairs;
        std::vector<std::array<std::list<my::Point>, 2>> lower_chain_pairs;
        const auto last_it = std::prev(middle_chain.end());
        int nonoverlapping_count = 0;

        while (*mid_it != *last_it) {
            while (*upper_it < *mid_it) {
                if (state == OVERLAPPING_TOP) {
                    state = OVERLAPPING_NEITHER;
                    HANDLE_OVERLAPPING_TOP // erase upper chain until now and update start reference pointer
                }
                upper_it = std::next(upper_it);
            }
            if (*mid_it == *upper_it) {
                switch (state) {
                case OVERLAPPING_BOTTOM:
                    HANDLE_OVERLAPPING_BOTTOM
                case OVERLAPPING_NEITHER:
                    upper_chain_pairs.emplace_back(std::array<std::list<my::Point>, 2>());
                    upper_chain_pairs.back()[0].splice(upper_chain_pairs.back()[0].begin(), upper_chain,
                                                       upper_chain.begin(), upper_it); // move upper head
                    upper_chain_pairs.back()[0].push_back(*upper_it); // must include end as well
                    std::copy(upper_ref_it, std::next(mid_it), std::back_inserter(upper_chain_pairs.back()[1]));
                // copy middle subchain
                    upper_ref_it = mid_it;
                    state = OVERLAPPING_TOP;
                    HANDLE_EDGE_LIST
                    break;
                case OVERLAPPING_TOP:
                    break; // do nothing
                }
                overlapping_it = upper_it;
                upper_it = std::next(upper_it);
            }
            else {
                while (*lower_it < *mid_it) {
                    if (state == OVERLAPPING_BOTTOM) {
                        state = OVERLAPPING_NEITHER;
                        HANDLE_OVERLAPPING_BOTTOM
                    }
                    lower_it = std::next(lower_it);
                }
                if (*mid_it == *lower_it) {
                    switch (state) {
                    case OVERLAPPING_TOP:
                        HANDLE_OVERLAPPING_TOP
                    case OVERLAPPING_NEITHER:
                        // std::copy(lower_ref_it, mid_it, std::back_inserter(pairs[0])); // copy middle subchain
                        // pairs[1].splice(pairs[1].begin(), lower_chain, lower_chain.begin(), lower_it); // move lower head
                        lower_chain_pairs.emplace_back(std::array<std::list<my::Point>, 2>());
                        std::copy(lower_ref_it, std::next(mid_it), std::back_inserter(lower_chain_pairs.back()[0]));
                    // copy middle subchain
                        lower_chain_pairs.back()[1].splice(lower_chain_pairs.back()[1].begin(), lower_chain,
                                                           lower_chain.begin(), lower_it); // move lower head
                        lower_chain_pairs.back()[1].push_back(*lower_it); // must include end as well
                        lower_ref_it = mid_it;
                        state = OVERLAPPING_BOTTOM;
                        HANDLE_EDGE_LIST
                        break;
                    case OVERLAPPING_BOTTOM:
                        break; // do nothing
                    }
                    overlapping_it = lower_it;
                    lower_it = std::next(lower_it);
                }
                else {
                    switch (state) {
                    case OVERLAPPING_BOTTOM:
                        HANDLE_OVERLAPPING_BOTTOM
                        break;
                    case OVERLAPPING_TOP:
                        HANDLE_OVERLAPPING_TOP
                        break;
                    case OVERLAPPING_NEITHER:
                        break;
                    }
                    state = OVERLAPPING_NEITHER;
                    nonoverlapping_count++;
                }
            }
            mid_it = std::next(mid_it);
        }
        while (*upper_it != *last_it) {
            if (state == OVERLAPPING_TOP) {
                state = OVERLAPPING_NEITHER;
                HANDLE_OVERLAPPING_TOP
            }
            upper_it = std::next(upper_it);
        }
        while (*lower_it != *last_it) {
            if (state == OVERLAPPING_BOTTOM) {
                state = OVERLAPPING_NEITHER;
                HANDLE_OVERLAPPING_BOTTOM
            }
            lower_it = std::next(lower_it);
        }
        // end case:
        switch (state) {
        case OVERLAPPING_BOTTOM:
            middle_chain.erase(middle_chain.begin(), upper_ref_it);
            upper_chain_pairs.emplace_back(std::array{std::move(upper_chain), std::move(middle_chain)});
            break;
        case OVERLAPPING_TOP:
            middle_chain.erase(middle_chain.begin(), lower_ref_it);
            lower_chain_pairs.emplace_back(std::array{std::move(middle_chain), std::move(lower_chain)});
            break;
        case OVERLAPPING_NEITHER: // whichever one is smaller will get std::move the other one will get a copy
            HANDLE_EDGE_LIST
            if (*upper_ref_it > *lower_ref_it) {
                upper_chain_pairs.emplace_back(std::array{std::move(upper_chain), std::list<my::Point>()});
                std::copy(upper_ref_it, middle_chain.end(), std::back_inserter(upper_chain_pairs.back()[1]));
                middle_chain.erase(middle_chain.begin(), lower_ref_it);
                lower_chain_pairs.emplace_back(std::array{std::move(middle_chain), std::move(lower_chain)});
            }
            else {
                lower_chain_pairs.emplace_back(std::array{std::list<my::Point>(), std::move(lower_chain)});
                std::copy(lower_ref_it, middle_chain.end(), std::back_inserter(lower_chain_pairs.back()[0]));
                middle_chain.erase(middle_chain.begin(), upper_ref_it);
                upper_chain_pairs.emplace_back(std::array{std::move(upper_chain), std::move(middle_chain)});
            }
            break;
        }
#ifndef NDEBUG
        for (auto& pair : upper_chain_pairs) {
            assert(pair[0].size() > 1);
            assert(pair[1].size() > 1);
            assert(*pair[0].begin() == *pair[1].begin());
            assert(*std::prev(pair[0].end()) == *std::prev(pair[1].end()));
        }
        for (auto& pair : lower_chain_pairs) {
            assert(pair[0].size() > 1);
            assert(pair[1].size() > 1);
            assert(*pair[0].begin() == *pair[1].begin());
            assert(*std::prev(pair[0].end()) == *std::prev(pair[1].end()));
        }

        int main_up_upper_chain_idx = 0, main_up_lower_chain_idx = 0,
            main_low_lower_chain_idx = 0, main_low_upper_chain_idx = 0;
        auto up_upper_pt = upper_chain_pairs[main_up_upper_chain_idx][0].begin();
        auto up_lower_pt = upper_chain_pairs[main_low_upper_chain_idx][1].begin();
        auto low_upper_pt = lower_chain_pairs[main_up_lower_chain_idx][0].begin();
        auto low_lower_pt = lower_chain_pairs[main_low_lower_chain_idx][1].begin();
        for (int i = 0; i < local_set_size; i++) {
            my::Point& p = local_set[i];
            if (!p.onEdge()) continue;
            bool p_found = false;
            while (p == *up_upper_pt) {
                p_found = true;
                up_upper_pt++;
                if (up_upper_pt == upper_chain_pairs[main_up_upper_chain_idx][0].end() && main_up_upper_chain_idx <
                    upper_chain_pairs.size() - 1) {
                    main_up_upper_chain_idx++;
                    up_upper_pt = upper_chain_pairs[main_up_upper_chain_idx][0].begin();
                }
            }
            while (p == *up_lower_pt) {
                p_found = true;
                up_lower_pt++;
                if (up_lower_pt == upper_chain_pairs[main_low_upper_chain_idx][1].end() && main_low_upper_chain_idx <
                    upper_chain_pairs.size() - 1) {
                    main_low_upper_chain_idx++;
                    up_lower_pt = upper_chain_pairs[main_low_upper_chain_idx][1].begin();
                }
            }
            while (p == *low_upper_pt) {
                p_found = true;
                low_upper_pt++;
                if (low_upper_pt == lower_chain_pairs[main_up_lower_chain_idx][0].end() && main_up_lower_chain_idx <
                    lower_chain_pairs.size() - 1) {
                    main_up_lower_chain_idx++;
                    low_upper_pt = lower_chain_pairs[main_up_lower_chain_idx][0].begin();
                }
            }
            while (p == *low_lower_pt) {
                p_found = true;
                low_lower_pt++;
                if (low_lower_pt == lower_chain_pairs[main_low_lower_chain_idx][1].end() && main_low_lower_chain_idx <
                    lower_chain_pairs.size() - 1) {
                    main_low_lower_chain_idx++;
                    low_lower_pt = lower_chain_pairs[main_low_lower_chain_idx][1].begin();
                }
            }
            assert(p_found);
        }
#endif


        delete[] local_set;
#pragma omp task
        process_chain_pairs(upper_chain_pairs, top_set);
#pragma omp task
        process_chain_pairs(lower_chain_pairs, bottom_set);
#pragma omp taskwait
    }

private:
    inline std::vector<std::vector<int>>& get_chain_output() const {
        const int thread_id = omp_get_thread_num();
        return chains[thread_id];
    }

    template <typename Iterator>
    bool trim_monotone_chains(Iterator& other_it, Iterator& mid_it) {
        bool run_once = false;
        while (*std::next(other_it) == *std::next(mid_it)) {
            other_it = std::next(other_it);
            mid_it = std::next(mid_it);
            run_once = true;
        }
        return run_once;
    }

    void process_chain_pairs(std::vector<std::array<std::list<my::Point>, 2>>& chain_pairs,
                             const std::vector<my::Point>& base_set) {
        int idx = 0;
        const my::Point* base_ptr = base_set.data();
        const my::Point* const base_end = base_ptr + base_set.size();
        for (int i = 0; i < chain_pairs.size(); i++) {
            if (i == chain_pairs.size() - 1) {
                idx = base_set.size() - (base_ptr - base_set.data());
            }
            else {
                while (base_ptr + idx < base_end && *(base_ptr + idx) < chain_pairs[i][0].back()) {
                    idx++;
                }
            }

            const my::Point* const lcl_base_end = base_ptr + idx;
            {
                auto& pair = chain_pairs[i];
                auto& sub_upper_chain = pair[0];
                auto& sub_lower_chain = pair[1];
                my::Point& end_point = sub_upper_chain.back();
                int size = (lcl_base_end - base_ptr) + sub_upper_chain.size() + sub_lower_chain.size() - 2;
                const auto sub_local_set = new my::Point[(lcl_base_end - base_ptr) + sub_upper_chain.size() +
                    sub_lower_chain.size() - 2];
                auto sub_upper_it = std::next(sub_upper_chain.begin());
                auto sub_lower_it = sub_lower_chain.begin();
                int point_idx = 0;
                const auto sub_upper_end = std::prev(sub_upper_chain.end());
                if (base_ptr < lcl_base_end) {
                    assert(*base_ptr > *sub_upper_chain.begin());
                    assert(*base_ptr > *sub_lower_chain.begin());
                }
                // Merge all three
                while (base_ptr < lcl_base_end || sub_upper_it != sub_upper_end || sub_lower_it != sub_lower_chain.
                    end()) {
                    if (base_ptr < lcl_base_end &&
                        (sub_upper_it == sub_upper_end || *base_ptr < *sub_upper_it) &&
                        (sub_lower_it == sub_lower_chain.end() || *base_ptr < *sub_lower_it)) {
                        auto prev_upper_it = std::prev(sub_upper_it);
                        auto prev_lower_it = std::prev(sub_lower_it);
                        assert(isBelowLine(*prev_upper_it, *sub_upper_it, *base_ptr));
                        assert(!isBelowLine(*prev_lower_it, *sub_lower_it, *base_ptr));
                        sub_local_set[point_idx++] = *base_ptr;
                        base_ptr++;
                    }
                    else if (sub_upper_it != sub_upper_end &&
                        (sub_lower_it == sub_lower_chain.end() || *sub_upper_it < *sub_lower_it)) {
                        sub_local_set[point_idx++] = *sub_upper_it++;
                    }
                    else if (sub_lower_it != sub_lower_chain.end()) {
                        sub_local_set[point_idx++] = *sub_lower_it++;
                    }
                }
                assert(point_idx == size);
                get_triangulation(sub_local_set, point_idx, sub_upper_chain, sub_lower_chain);
            }
            base_ptr = lcl_base_end;
            idx = 0;
        }
    }

    struct Circle {
        REAL x, y, r;
    };

    static Circle circumcircle(const my::Point& a, const my::Point& b, const my::Point& c) {
        REAL A = b.x - a.x;
        REAL B = b.y - a.y;
        REAL C = c.x - a.x;
        REAL D = c.y - a.y;

        REAL E = A * (a.x + b.x) + B * (a.y + b.y);
        REAL F = C * (a.x + c.x) + D * (a.y + c.y);
        REAL G = 2.0 * (A * (c.y - b.y) - B * (c.x - b.x));


        REAL cx = (D * E - B * F) / G;
        REAL cy = (A * F - C * E) / G;

        REAL dx = cx - a.x;
        REAL dy = cy - a.y;
        REAL radius = std::sqrt(dx * dx + dy * dy);

        return Circle{cx, cy, radius};
    }

    static bool isInside(const Circle& circle, const my::Point& p) {
        return (p.x - circle.x) * (p.x - circle.x) + (p.y - circle.y) * (p.y - circle.y) < circle.r * circle.r;
    }

    static bool isBelowLine(const my::Point& a, const my::Point& b, const my::Point& p) {
        return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x) < 0;
    }
};


#endif //CS564PROJECT_DELAUNAY_TRIANGULATION_H
