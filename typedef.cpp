//
// Created by serdar on 4/5/25.
//
#include "typedef.h"
#include <cstdlib>

glm::vec3 get_random_color(unsigned int seed) {
    if (seed != UINT32_MAX) {
        srand(seed);
    }
    uint8_t r, g, b;
    do {
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
    } while (r + g + b > 600);
    return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}


