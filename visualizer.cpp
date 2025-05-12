//
// Created by serdar on 3/21/25.
//

#include "visualizer.h"

#include <cmath>


Renderer::Renderer(int width, int height, const std::string& title, delaunay_triangulation* dt, std::vector<sf::Vector2f>* points)
        : dt(dt), points(points) {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    this->width = width;
    this->height = height;
    sf::Vector2u size(width, height);
    window.create(sf::VideoMode(size), title, sf::Style::Default, sf::State::Windowed, settings);

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();
    // find the bounding box
    for (const auto& p : dt->points) {
        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }
    scale = std::min(width / (max_x - min_x), height / (max_y - min_y)) * 0.3;
    offset = sf::Vector2f(-min_x * scale, -min_y * scale);
}

void Renderer::generate_delaunay() {
    window.clear(sf::Color::White);
    if (polygon_idx == -1) {
        generate_impl_delaunay();
    } else {
        render_polygon();
    }
    window.display();
}

inline sf::Vector2f get_inverted(const my::Point& vec, const my::Point& p1) {
    const float l2n = std::sqrt((vec.x - p1.x) * (vec.x - p1.x) + (vec.y - p1.y) * (vec.y - p1.y));
    return sf::Vector2f((vec.x - p1.x) / l2n, (vec.y - p1.y) / l2n);
}

void Renderer::render_polygon() {
    const std::list<my::Point>& upper_chain = dt->chain_pairs[polygon_idx].first;
    const std::list<my::Point>& lower_chain = dt->chain_pairs[polygon_idx].second;

    int color_idx = 0;
    for (int k = 0; k < 2; k++) {
        const std::list<my::Point>& chain = k == 0 ? upper_chain : lower_chain;
        sf::VertexArray chain_line(sf::PrimitiveType::LineStrip, chain.size());
        auto p = chain.begin();
        for (int i = 0; i < chain.size(); i++) {
            sf::CircleShape point(point_radius);
            point.setFillColor(get_random_color(color_idx++));
            sf::Vector2f rad_offset(-point_radius, -point_radius);
            sf::Vector2f vec2(p->x, p->y);
            point.setPosition((vec2 * scale) + offset + rad_offset);
            window.draw(point);
            chain_line[i].position = (vec2 * scale) + offset;
            chain_line[i].color = k == 0 ? sf::Color::Red : sf::Color::Blue;
            ++p;
        }
        window.draw(chain_line);
    }

    const std::list<my::Point>& long_chain = upper_chain.size() > lower_chain.size() ? upper_chain : lower_chain;
    const std::list<my::Point>& short_chain = upper_chain.size() > lower_chain.size() ? lower_chain : upper_chain;
    const std::list<my::Point>& selected_chain = short_chain.size() > 2 ? short_chain : long_chain;
    auto it = selected_chain.begin();
    for (int i = 0; i < selected_chain.size() / 2; i++) {
        ++it;
    }
    const my::Point& p1 = *it;
    const my::Point& prev = *std::prev(it);
    const my::Point& next = *std::next(it);
    sf::Vector2f* inverted_points[2];
    inverted_points[0] = new sf::Vector2f[upper_chain.size()];
    inverted_points[1] = new sf::Vector2f[lower_chain.size()];

    float inv_min_y = std::numeric_limits<float>::max(),
            inv_max_y = std::numeric_limits<float>::lowest(),
    inv_max_x = std::numeric_limits<float>::lowest(),
            inv_min_x = std::numeric_limits<float>::max();
    for (int k = 0; k < 2; k++) {
        const std::list<my::Point>& chain = k == 0 ? upper_chain : lower_chain;
        auto it = chain.begin();
        for (int i = 0; i < chain.size(); i++) {
            if (*it != p1)
                inverted_points[k][i] = get_inverted(*it, p1);
            else {
                inverted_points[k][i] = sf::Vector2f(0, 0);
            }
            if (inverted_points[k][i].x < inv_min_x) {
                inv_min_x = inverted_points[k][i].x;
            }
            if (inverted_points[k][i].x > inv_max_x) {
                inv_max_x = inverted_points[k][i].x;
            }
            if (inverted_points[k][i].y < inv_min_y) {
                inv_min_y = inverted_points[k][i].y;
            }
            if (inverted_points[k][i].y > inv_max_y) {
                inv_max_y = inverted_points[k][i].y;
            }
            ++it;
        }
    }
    float min_x = upper_chain.front().x, max_x = upper_chain.back().x,
        min_y = std::numeric_limits<float>::max(),
        max_y = std::numeric_limits<float>::lowest();

    for (int k = 0; k < 2; k++) {
        const std::list<my::Point>& chain = k == 0 ? upper_chain : lower_chain;
        for (int i = 0; i < chain.size(); i++) {
            if (chain.front().y < min_y) {
                min_y = chain.front().y;
            }
            if (chain.back().y > max_y) {
                max_y = chain.back().y;
            }
        }
    }
    const float inv_scale = std::max((max_x - min_x)/(inv_max_x - inv_min_x), (max_y - min_y)/(inv_max_y - inv_min_y));
    const sf::Vector2f inv_offset(max_x - inv_min_x + 10, max_y - inv_max_y);
    for (int k = 0; k < 2; k++) {
        const std::list<my::Point>& chain = k == 0 ? upper_chain : lower_chain;
        for (int i = 0; i < chain.size(); i++) {
            inverted_points[k][i] = (inverted_points[k][i] * inv_scale + inv_offset) * scale + offset;
        }
    }

    color_idx = 0;
    for (int k = 0; k < 2; k++) {
        const std::list<my::Point>& chain = k == 0 ? upper_chain : lower_chain;
        sf::VertexArray chain_line(sf::PrimitiveType::LineStrip, chain.size());
        for (int i = 0; i < chain.size(); i++) {
            const sf::Vector2f& vec2 = inverted_points[k][i];
            sf::CircleShape point(point_radius);
            point.setFillColor(get_random_color(color_idx++));
            const sf::Vector2f rad_offset(-point_radius, -point_radius);
            point.setPosition(vec2 + rad_offset);
            window.draw(point);
            chain_line[i].position = vec2;
            chain_line[i].color = k == 0 ? sf::Color::Red : sf::Color::Blue;
        }
        window.draw(chain_line);
    }
    const sf::Vector2f inv_next = (get_inverted(next, p1) * inv_scale + inv_offset) * scale + offset;
    const sf::Vector2f inv_prev = (get_inverted(prev, p1) * inv_scale + inv_offset) * scale + offset;
    sf::Vertex line[] = {
            sf::Vertex(inv_next, sf::Color::Green),
            sf::Vertex(inv_prev, sf::Color::Green),
    };
    window.draw(line, 2, sf::PrimitiveType::Lines);

}


void Renderer::generate_impl_delaunay() {
    for (auto& vec: dt->points){
        sf::CircleShape point(point_radius);
        point.setFillColor(point_color);
        sf::Vector2f rad_offset(-point_radius, -point_radius);
        point.setPosition((vec * scale) + offset + rad_offset);
        window.draw(point);
    }
//    for (auto& vec: dt->left_set) {
//        sf::CircleShape point(point_radius);
//        point.setFillColor(sf::Color::Green);
//        sf::Vector2f rad_offset(-point_radius, -point_radius);
//        sf::Vector2f vec2(vec.x, vec.y);
//        point.setPosition((vec2 * scale) + offset + rad_offset);
//        window.draw(point);
//    }
//
//    for (auto& vec: dt->right_set) {
//        sf::CircleShape point(point_radius);
//        point.setFillColor(sf::Color::Red);
//        sf::Vector2f rad_offset(-point_radius, -point_radius);
//        sf::Vector2f vec2(vec.x, vec.y);
//        point.setPosition((vec2 * scale) + offset + rad_offset);
//        window.draw(point);
//    }
    for (int k = 0; k < dt->chains.size(); k++) {
        std::vector<my::Point> chain = dt->chains[k];
        sf::Color color = get_random_color(k);
        sf::VertexArray chain_line(sf::PrimitiveType::LineStrip, chain.size());
        for (int i = 0; i < chain.size(); i++) {
            sf::Vector2f vec2(chain[i].x, chain[i].y);
            chain_line[i].position = (vec2 * scale) + offset;
            chain_line[i].color = color;
        }
        window.draw(chain_line);
        // float med_y = dt->medians_y[k];
        // if (med_y == INFINITY) {
        //     continue;
        // }
//        color.a = 120;
//        sf::Vertex line[] = {
//                sf::Vertex(sf::Vector2f(-1000, med_y) * scale + offset , color),
//                sf::Vertex(sf::Vector2f(1000, med_y) * scale + offset , color),
//        };
//        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    for (int k = 0; k < dt->medians_y.size(); k++)
    {
        float med_y = dt->medians_y[k];
        if (med_y == INFINITY) {continue;}
        sf::Color color = get_random_color(k + 2);
        color.a = 120;
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(-1000, med_y) * scale + offset , color),
            sf::Vertex(sf::Vector2f(1000, med_y) * scale + offset , color),
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }

//    int hullLength = points->size();
//    for (int i = 0; i < hullLength - 1; i++) {
//        int next = (i + 1) % hullLength;
//        sf::Vertex line[] = {
//                sf::Vertex(((*points)[i] * scale) + offset, line_color),
//                sf::Vertex(((*points)[next] * scale) + offset, line_color)
//        };
//        window.draw(line, 2, sf::PrimitiveType::Lines);
//    }
}

// void Renderer::generate_test_delaunay() {
//     for (auto it = dt_test->finite_faces_begin(); it != dt_test->finite_faces_end(); ++it) {
//         Point_2 p1 = it->vertex(0)->point();
//         Point_2 p2 = it->vertex(1)->point();
//         Point_2 p3 = it->vertex(2)->point();
//         sf::ConvexShape triangle(3);
//         triangle.setPoint(0, sf::Vector2f(p1.x() * scale, p1.y() * scale) + offset);
//         triangle.setPoint(1, sf::Vector2f(p2.x() * scale, p2.y() * scale) + offset);
//         triangle.setPoint(2, sf::Vector2f(p3.x() * scale, p3.y() * scale) + offset);
//         triangle.setFillColor(sf::Color::Transparent);
//         triangle.setOutlineColor(line_color);
//         triangle.setOutlineThickness(line_thickness);
//         window.draw(triangle);
//         for (int i = 0; i < 3; i++) {
//             sf::CircleShape point(point_radius);
//             point.setFillColor(point_color);
//             sf::Vector2f rad_offset(-point_radius, -point_radius);
//             point.setPosition(triangle.getPoint(i) + rad_offset);
//             window.draw(point);
//         }
//         if (show_circles) {
//             Point_2 c = CGAL::circumcenter(SFtoCGAL(triangle.getPoint(0)), SFtoCGAL(triangle.getPoint(1)),
//                                            SFtoCGAL(triangle.getPoint(2)));
//             sf::Vector2f csf(c.x(), c.y());
//             sf::CircleShape circle(CGAL::sqrt(CGAL::squared_distance(SFtoCGAL(triangle.getPoint(0)), c)));
//             circle.setFillColor(sf::Color::Transparent);
//             circle.setOutlineColor(sf::Color::Green);
//             circle.setOutlineThickness(line_thickness);
//             circle.setPosition(csf - sf::Vector2f(circle.getRadius(), circle.getRadius()));
//             window.draw(circle);
//         }
//     }
// }


void Renderer::game_loop() {
    bool mouse_button_held = false;
    sf::Vector2i initial_mouse_pos;
    // Main loop
    while (window.isOpen()) {
        if (auto event = window.pollEvent(); event.has_value()) {
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    show_circles = !show_circles;
                    generate_delaunay();
                } else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                    polygon_idx = (polygon_idx + 1) % dt->chains.size();
                    generate_delaunay();
                } else if (keyPressed->scancode == sf::Keyboard::Scancode::C) {
                    polygon_idx = -1;
                    generate_delaunay();
                }
            }
            if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
                int dx = mouseWheelScrolled->position.x - width / 2;
                int dy = mouseWheelScrolled->position.y - height / 2;
                const float zoom_factor = 1.1f;
                if (mouseWheelScrolled->delta > 0) {
//                    offset -= sf::Vector2f(dx, dy) * (zoom_factor - 1);
                    scale *= zoom_factor;
                } else {
//                    offset += sf::Vector2f(dx, dy) * (1 - 1 / zoom_factor);
                    scale /= zoom_factor;
                }
                generate_delaunay();
            }
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                width = resized->size.x;
                height = resized->size.y;
                generate_delaunay();
            } else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    mouse_button_held = true;
                    initial_mouse_pos = mouseButtonPressed->position;
                }
            } else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    mouse_button_held = false;
                }
            } else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                if (mouse_button_held)
                {
                    offset += (sf::Vector2f) (mouseMoved->position - initial_mouse_pos);
                    initial_mouse_pos = mouseMoved->position;
                    generate_delaunay();
                }
            }

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        } else {
            sleep(sf::milliseconds(10));
        }
    }
}
