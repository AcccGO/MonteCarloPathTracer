// #include "sphere.h"

// #include "util.h"

// double Sphere::area()
// {
//     return 4 * PI * this->radius * this->radius;
// }

// glm::dvec3 Sphere::sample() const
// {
//     double rndx, rndy, rndz;
//     double magnitude;
//     do {
//         rndx      = 2.0 * (double)rand() / RAND_MAX - 1.0;
//         rndy      = 2.0 * (double)rand() / RAND_MAX - 1.0;
//         rndz      = 2.0 * (double)rand() / RAND_MAX - 1.0;
//         magnitude = sqrt(rndx * rndx + rndy * rndy + rndz * rndz);
//     } while (magnitude > 1.0);

//     glm::dvec3 rnd_point(rndx / magnitude, rndy / magnitude, rndz / magnitude);
//     rnd_point = glm::normalize(rnd_point);
//     return rnd_point * radius + center;
// }

// Sphere::Sphere(glm::dvec3 center = glm::dvec3(0.0), double radius = 1.0)
// {
//     this->center = center;
//     this->radius = radius;

//     this->is_light = false;
// }

// bool Sphere::intersect(Ray &r, Hit &h)
// {
//     glm::dvec3 direction = r.getDirection();
//     glm::dvec3 origin    = r.getOrigin();

//     double a     = glm::dot(direction, direction);
//     double b     = 2.0f * glm::dot(direction, (origin - center));
//     double c     = glm::length(origin - center) * glm::length(origin - (center)) - radius * radius;
//     double delta = b * b - 4 * a * c;

//     if (delta >= 0) {
//         double d  = sqrt(delta);
//         double t1 = (-b - d) / (a * 2.0f);
//         double t2 = (-b + d) / (a * 2.0f);

//         if (!r.isValidT(t1))
//             t1 = -1;
//         if (!r.isValidT(t2))
//             t2 = -1;

//         if (t1 > 0 && t2 > 0) {
//             h.intersection_object       = this;
//             h.intersection_point        = r.pointAtParameter(t1);
//             h.intersection_point_normal = glm::normalize(h.intersection_point - this->center);
//             h.t                         = t1;
//             r.setMaxT(t1);

//             return true;
//         } else if (t1 < 0 && t2 > 0) {
//             h.intersection_object       = this;
//             h.intersection_point        = r.pointAtParameter(t2);
//             h.intersection_point_normal = -glm::normalize(h.intersection_point - this->center);
//             h.t                         = t2;
//             r.setMaxT(t2);

//             return true;
//         }
//     }
//     return false;
// }

// AABB Sphere::cal_AABB()
// {
//     glm::dvec3 _min(this->center - this->radius);
//     glm::dvec3 _max(this->center + this->radius);

//     return AABB(_min, _max);
// }