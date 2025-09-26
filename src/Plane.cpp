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

#include "Plane.h"
#include <limits>


//== CLASS DEFINITION =========================================================



Plane::Plane(const vec3& _center, const vec3& _normal)
: center(_center), normal(_normal)
{
}


//-----------------------------------------------------------------------------


bool
Plane::
intersect(const Ray& _ray,
          vec3&      _intersection_point,
          vec3&      _intersection_normal,
          double&    _intersection_t ) const
{

        const vec3 &dir = _ray.direction;
        const vec3   oc = _ray.origin - center;

        //compute the intersection of the plane with `_ray`
        double denom = dot(_intersection_normal, dir);

        //check if ray is parallel to plane
        if (fabs(denom) < 0.000000001) return false;

        double t = -dot(_intersection_normal,oc) / denom;

        //check if ray intersects plane behind origin
        if (t <= 0) return false;

        _intersection_t      = t;
        _intersection_point  = _ray.origin + t * dir;

        //normal at intersection point is simply plane's normal
        _intersection_normal = normal;

        return true;
}


//=============================================================================
