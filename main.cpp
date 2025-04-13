#include <iostream>
#include <vector>
#include "typedef.h"
#include "plane_sampler.h"
#include "visualizer.h"

// Function to lift 2D points to 3D paraboloid
std::vector<Point_3> lift_points(const std::vector<Point_2>& points) {
    std::vector<Point_3> lifted_points;
    for (const auto& p : points) {
        double x = p.x();
        double y = p.y();
        lifted_points.push_back(Point_3(x, y, x * x + y * y));
    }
    return lifted_points;
}

// Function to compute the Delaunay triangulation using the lifting method
Delaunay compute_delaunay(const std::vector<Point_2>& points) {
    // Lift points to 3D
    std::vector<Point_3> lifted_points = lift_points(points);

    // Compute the 3D convex hull
    Polyhedron_3 poly;
    CGAL::convex_hull_3(lifted_points.begin(), lifted_points.end(), poly);

    // Extract the lower hull (Delaunay triangulation)
    std::vector<Point_2> delaunay_points;
    for (auto it = poly.vertices_begin(); it != poly.vertices_end(); ++it) {
        Point_3 p = it->point();
        delaunay_points.push_back(Point_2(p.x(), p.y()));
    }

    // Compute the Delaunay triangulation in 2D
    Delaunay dt;
    dt.insert(delaunay_points.begin(), delaunay_points.end());

    return dt;
}

int main() {

    // Example point set
//    std::vector<Point_2> points = uniform_plane(10, -10, 10, -10, 10);
    std::vector<Point_2> points = gaussian_plane(310, 20, 0);
//    std::vector<Point_2> points = circle_plane(7, 10);
    // Compute Delaunay triangulation

    // Visualize the Delaunay triangulation
    char text[100];
    sprintf(text, "Delaunay Triangulation with %d points", (int)points.size());
    #ifdef TEST_MODE
    Renderer renderer(800, 800, text, &dt);
    Delaunay dt = compute_delaunay(points);
    #else
    std::vector<sf::Vector2f> sf_points;
    sf_points.reserve(points.size());
    for (const auto& p : points) {
        sf_points.emplace_back(p.x(), p.y());
    }
    std::sort(sf_points.begin(), sf_points.end(), [](const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    delaunay_triangulation dt(sf_points);
    std::vector<sf::Vector2f> hull = dt.solve();
    Renderer renderer(800, 800, text, &dt, &hull);
    #endif
    renderer.generate_delaunay();
    renderer.game_loop();
    return 0;
}