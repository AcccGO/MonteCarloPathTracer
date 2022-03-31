#include "frame.h"

#include "math.h"

Frame::Frame()
{
    mx = glm::dvec3(1, 0, 0);
    my = glm::dvec3(0, 1, 0);
    mz = glm::dvec3(0, 0, 1);
}

Frame::Frame(const glm::dvec3& x, const glm::dvec3& y, const glm::dvec3& z)
    : mx(x), my(y), mz(z)
{
}

Frame::Frame(glm::dvec3 n)
    : mz(n)
{
    coordinateSystem(mz, mx, my);
}

void Frame::setFromZ(const glm::dvec3& z)
{
    this->mz = z;
    coordinateSystem(mz, mx, my);
}

glm::dvec3 Frame::toWorld(const glm::dvec3& a) const
{
    return mx * a.x + my * a.y + mz * a.z;
}

glm::dvec3 Frame::toLocal(const glm::dvec3& a) const
{
    return glm::dvec3(glm::dot(a, mx), glm::dot(a, my), glm::dot(a, mz));
}

double Frame::cosTheta(const glm::dvec3& v)
{
    return v.z;
}