//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_TYPEDEF_H
#define CS564PROJECT_TYPEDEF_H
#include <SFML/Graphics.hpp>

sf::Color get_random_color(unsigned int seed = UINT32_MAX);


namespace my
{
    struct Point
    {
        float x, y,z;
        bool onEdge;

        Point() = default;

        Point(const float x, const float y) : x(x), y(y), z(0), onEdge(false) {}

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
    };

}



#endif //CS564PROJECT_TYPEDEF_H
