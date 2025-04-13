//
// Created by serdar on 3/21/25.
//

#ifndef CS564PROJECT_TYPEDEF_H
#define CS564PROJECT_TYPEDEF_H
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polyhedron_3.h>
#include <SFML/Graphics.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Point_3 Point_3;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef CGAL::Projection_traits_xy_3<K> Gt;
typedef CGAL::Convex_hull_traits_3<K> Hull_traits_3;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;

sf::Color get_random_color(unsigned int seed = UINT32_MAX);

sf::Vector2f CGALtoSF(const Point_2 p);

Point_2 SFtoCGAL(const sf::Vector2f p);

#endif //CS564PROJECT_TYPEDEF_H
