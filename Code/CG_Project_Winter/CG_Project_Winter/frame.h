#pragma once
#include <glm.hpp>

class Frame
{
public:
    Frame();
    Frame(const glm::dvec3& x, const glm::dvec3& y, const glm::dvec3& z);
    Frame(glm::dvec3 n);

    void          setFromZ(const glm::dvec3& z);
    glm::dvec3    toWorld(const glm::dvec3& a) const;
    glm::dvec3    toLocal(const glm::dvec3& a) const;
    static double cosTheta(const glm::dvec3& v);

public:
    glm::dvec3 mx, my, mz;
};