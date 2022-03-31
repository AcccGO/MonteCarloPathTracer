#pragma once
#include <glm.hpp>

#include "math.h"
class Emitter
{
public:
    size_t         shapeID;
    double         area;
    glm::dvec3     radiance;
    Distribution1D faceAreaDistribution;
    double         average_power;

    bool operator==(const Emitter& other) const;
};