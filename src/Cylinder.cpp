//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================
//== INCLUDES =================================================================
#include "Cylinder.h"
#include "SolveQuadratic.h"
#include <array>
#include <cmath>
    //== IMPLEMENTATION =========================================================
    bool
        Cylinder::
    intersect(const Ray&  _ray,
              vec3&       _intersection_point,
              vec3&       _intersection_normal,
              double&     _intersection_t) const
{
    /** \todo
     * - compute the first valid intersection `_ray` with the cylinder
     *   (valid means in front of the viewer: t > 0)
     * - store intersection point in `_intersection_point`
     * - store ray parameter in `_intersection_t`
     * - store normal at *intersection*point in `_intersection_normal`.
     * - return whether there is an intersection with t > 0
    */
    std::array<double, 2> t;
    double h;
    _intersection_t = NO_INTERSECTION;
    vec3 p;

    vec3 m = _ray.origin - center;
    vec3 d = _ray.direction;

    vec3 m_perp = m - dot(m, axis) * axis;
    vec3 d_perp = d - dot(d, axis) * axis;

    double A = dot(d_perp, d_perp);
    double B = 2.0 * dot(m_perp, d_perp);
    double C = dot(m_perp, m_perp) - radius*radius;

    size_t solutions = solveQuadratic(A,B,C,t);

    if (solutions == 0) {
        return false;
    }
    if (solutions == 2 && t[0] > t[1]) {
        std::swap(t[0], t[1]);
    }

    for (size_t i = 0; i < solutions; ++i) {
        if (t[i] > 0) {
            p = _ray.origin + t[i] * _ray.direction;
            h = dot((p - center), axis);

            // is intersection within cylinder height?
            if (h >= -height/2 && h <= height/2) {
                _intersection_t = t[i];
                _intersection_point = p;

                // (x - c) - ((x - c) * v)* v
                vec3 radial = (_intersection_point - center)
                              - dot((_intersection_point - center), axis) * axis;

                vec3 normal = normalize(radial);

                // ray is coming from outside -> invert normal
                // ray is coming from inside -> normal
                if (dot(_ray.direction, normal) > 0) {
                    _intersection_normal = -normal;

                } else {
                    _intersection_normal = normal;
                }

                return true;
            }
        }
    }

    return false;
}
