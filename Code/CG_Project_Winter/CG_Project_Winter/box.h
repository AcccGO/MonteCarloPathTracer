// #pragma once

// #include "object3d.h"

// class Box : public Object3D
// {
// public:
//     Box(glm::dvec3 corner1, glm::dvec3 corner2);
//     Box(glm::dvec3 min_c, double x, double y, double z);

//     virtual bool intersect(Ray &r, Hit &h);
//     virtual AABB cal_AABB();

// private:
//     glm::dvec3 min_corner;
//     glm::dvec3 max_corner;

//     double x_length, y_length, z_length;
// };