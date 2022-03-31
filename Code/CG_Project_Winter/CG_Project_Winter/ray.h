#pragma once
#include <glm.hpp>
#include <iostream>
#include <vec3.hpp>
using namespace std;

class Ray
{
public:
    Ray();
    Ray(const glm::dvec3& orig, const glm::dvec3& dir);
    Ray(const Ray& r);
    ~Ray();

    const glm::dvec3& getOrigin() const;
    const glm::dvec3& getDirection() const;
    void              setOrigin(glm::dvec3 pos);
    void              setDirection(glm::dvec3 dir);

    const double& getMaxT() const;
    const double& getMinT() const;
    void          setMaxT(double tmax);
    void          setMinT(double tmin);
    void          setT(double tmin, double tmax);
    bool          isValidT(double t);

    glm::dvec3 pointAtParameter(double t);

private:
    glm::dvec3 origin;
    glm::dvec3 direction;
    double     t_min, t_max;
};

inline ostream& operator<<(ostream& os, const Ray& r)
{
    auto o = r.getOrigin();
    auto d = r.getDirection();
    os << "Ray <o:(" << o.x << "," << o.y << "," << o.z << "), d:(" << d.x << "," << d.y << "," << d.z << ") >";
    return os;
}