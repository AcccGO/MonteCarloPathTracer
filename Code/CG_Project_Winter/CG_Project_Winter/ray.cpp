#include "ray.h"

#include <iostream>
#include <string>

#include "math.h"

Ray::Ray()
{
    origin    = glm::dvec3(0.0);
    direction = glm::dvec3(0.0);
    t_min     = EPS;
    t_max     = MAX_INFINITY_POSITIVE;
}

Ray::Ray(const glm::dvec3& orig, const glm::dvec3& dir)
{
    origin    = orig;
    direction = glm::normalize(dir);
    t_min     = EPS;
    t_max     = MAX_INFINITY_POSITIVE;
}

Ray::Ray(const Ray& r)
{
    *this           = r;
    this->direction = glm::normalize(this->direction);
}

Ray::~Ray()
{
}

const glm::dvec3& Ray::getOrigin() const
{
    return origin;
}

const glm::dvec3& Ray::getDirection() const
{
    return direction;
}

glm::dvec3 Ray::pointAtParameter(double t)
{
    if (isValidT(t))
        return origin + direction * t;
    else {
        throw std::runtime_error("invalid t:" + std::to_string(t));
    }
}

void Ray::setOrigin(glm::dvec3 pos)
{
    this->origin = pos;
}

void Ray::setDirection(glm::dvec3 dir)
{
    this->direction = glm::normalize(dir);
}

const double& Ray::getMaxT() const
{
    return t_max;
}

const double& Ray::getMinT() const
{
    return t_min;
}

void Ray::setMaxT(double tmax)
{
    this->t_max = tmax;
}

void Ray::setMinT(double tmin)
{
    this->t_min = tmin;
}

void Ray::setT(double tmin, double tmax)
{
    setMaxT(tmax);
    setMinT(tmin);
}

bool Ray::isValidT(double t)
{
    if (t < t_min || t > t_max)
        return false;
    return true;
}