#include <iostream>
#include <vector>
#include "typedef.h"
#include "plane_sampler.h"
#include "visualizer.h"
#include <algorithm>

int main() {

    // Example point set
//    std::vector<Point_2> points = uniform_plane(10, -10, 10, -10, 10);
    const std::vector<my::Point> points = gaussian_plane(310, 20, 0);
    // std::vector<Point_2> points = circle_plane(7, 10);
    // Compute Delaunay triangulation
    // std::vector<Point_2> points{Point_2{-2, 0}, Point_2{-1.5, -1}, Point_2{-1, -0.5}, Point_2{1, -0.5},
    //     Point_2{2, 0}, Point_2{0, 2}, Point_2{0, -2}};

    // Visualize the Delaunay triangulation
    char text[100];
    sprintf(text, "Delaunay Triangulation with %d points", (int)points.size());
    std::vector<sf::Vector2f> sf_points;
    sf_points.reserve(points.size());
    for (const auto& p : points) {
        sf_points.emplace_back(p.x, p.y);
    }
    std::ranges::sort(sf_points, [](const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    delaunay_triangulation dt(sf_points);
    std::vector<sf::Vector2f> hull = dt.solve();
    Renderer renderer(800, 800, text, &dt, &hull);
    renderer.generate_delaunay();
    renderer.game_loop();
    return 0;
}