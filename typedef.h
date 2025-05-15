//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_TYPEDEF_H
#define CS564PROJECT_TYPEDEF_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#ifdef DOUBLE_REAL
#define REAL double
#define Vector2 glm::dvec2
#else
#define REAL float
#define Vector2 glm::vec2
#endif

glm::vec3 get_random_color(unsigned int seed = UINT32_MAX);

constexpr unsigned int last_bit = 1 << 31;
namespace my
{
    static int32_t idx_count = 0;
    struct Point
    {
        REAL x, y, z;
        uint32_t id;

        Point() = default;
        // supports up to 2^31 - 1 points
        Point(const REAL x, const REAL y) : x(x), y(y), z(0), id(idx_count++){}

        bool operator==(const Point& p) const
        {
            return p.x == x && p.y == y;
        }

        bool operator!=(const Point& p) const
        {
            return p.x != x || p.y != y;
        }

        bool operator<(const Point& p) const
        {
            return x < p.x || (x == p.x && y < p.y);
        }

        bool operator>(const Point& p) const
        {
            return x > p.x || (x == p.x && y > p.y);
        }

        bool operator<=(const Point& p) const
        {
            return x < p.x || (x == p.x && y <= p.y);
        }

        Point& operator=(const Point& p)
        {
            x = p.x;
            y = p.y;
            z = p.z; // maybe remove?
            id = p.id;
            return *this;
        }

        // the last bit is used to mark the point as on the edge
        bool onEdge() const {
            return id & last_bit;
        }

        void setOnEdge() {
            id |= last_bit;
        }

        int32_t getId() const {
            return id & ~last_bit;
        }
    };
}


#endif //CS564PROJECT_TYPEDEF_H
