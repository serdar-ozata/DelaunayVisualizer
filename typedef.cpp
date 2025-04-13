//
// Created by serdar on 4/5/25.
//
#include "typedef.h"

sf::Vector2f CGALtoSF(const Point_2 p) {
    return sf::Vector2f(p.x(), p.y());
}

Point_2 SFtoCGAL(const sf::Vector2f p) {
    return Point_2(p.x, p.y);
}
sf::Color get_random_color(unsigned int seed) {
    if (seed != UINT32_MAX) {
        srand(seed);
    }
    uint8_t r, g, b;
    do {
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
    } while (r + g + b > 600);
    return sf::Color(r, g, b);
}