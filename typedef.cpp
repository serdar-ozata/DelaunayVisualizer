//
// Created by serdar on 4/5/25.
//
#include "typedef.h"

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

constexpr unsigned int last_bit = 1 << 31;
