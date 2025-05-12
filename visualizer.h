//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_VISUALIZER_H
#define CS564PROJECT_VISUALIZER_H

#include <SFML/Graphics.hpp>
#include "typedef.h"
#include "delaunay_triangulation.h"

class Renderer {
    sf::RenderWindow window;
    // parameters
    int width, height;
    sf::Vector2f offset;
    float scale;
    // End parameters
    bool show_circles = false;
    const int point_radius = 5;
    const int line_thickness = 2;
    const sf::Color point_color = sf::Color::Red;
    const sf::Color line_color = sf::Color::Blue;
    int polygon_idx = -1;

    delaunay_triangulation* dt;
    std::vector<sf::Vector2f>* points;

public:
    Renderer(int width, int height, const std::string& title, delaunay_triangulation* dt, std::vector<sf::Vector2f>* points);

    void generate_delaunay();

    void render_polygon();

    void game_loop();
private:

    void generate_impl_delaunay();
};

#endif //CS564PROJECT_VISUALIZER_H
