#pragma once

#include <vec3.hpp>

#include "frame.h"
#include "ray.h"

// https://blog.csdn.net/qq_22488067/article/details/73195621
class Triangle;

class Hit
{
public:
    Hit();
    Hit(double _t, Triangle *m);
    Hit(const Hit &h);
    ~Hit();

    double     t;  // Direction distance.
    Triangle  *intersection_object;
    glm::dvec3 intersection_point;
    glm::dvec3 intersection_point_normal;

    unsigned int sampled_component;
    unsigned int sampled_type;
    glm::dvec3   wo, wi;
    double       u, v;
    Frame        frame_ng;
    Frame        frame_ns;
};

inline ostream &operator<<(ostream &os, const Hit &h)
{
    os << "Hit <t:" << h.t << ">";
    return os;
}
