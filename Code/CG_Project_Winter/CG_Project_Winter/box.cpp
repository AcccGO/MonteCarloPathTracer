// #include "box.h"

// glm::dvec3 Box::sample() const
// {
// }

// Box::Box(glm::dvec3 corner1, glm::dvec3 corner2)
// {
//     if (!(corner2.x > corner1.x && corner2.y > corner1.y && corner2.z > corner1.z))
//         throw std::runtime_error("Box: corner2 is smaller than corner1");

//     this->min_corner = corner1;
//     this->x_length   = corner2.x - corner1.x;
//     this->y_length   = corner2.y - corner2.y;
//     this->z_length   = corner2.z - corner2.z;
// }

// Box::Box(glm::dvec3 min_c, double x, double y, double z)
// {
//     this->min_corner = min_c;
//     this->x_length   = x;
//     this->y_length   = y;
//     this->z_length   = z;
//     this->max_corner = min_c + glm::dvec3(x, y, z);
// }

// bool Box::intersect(Ray &r, Hit &h)
// {
//     double t_near = MIN_INFINITY_NEGATIVE;
//     double t_far  = MAX_INFINITY_POSITIVE;

//     glm::dvec3 ray_dir = r.getDirection();
//     glm::dvec3 ray_pos = r.getOrigin();

//     glm::dvec3 hit_normal;

//     // Inter.
//     for (auto i = 0; i < 3; i++) {
//         bool overturn_normal = false;

//         double     t1, t2;
//         double     ro, rd;
//         double     x1, x2;
//         glm::dvec3 n;

//         if (i == 0) {
//             ro = ray_pos.x;
//             rd = ray_dir.x;
//             x1 = min_corner.x;
//             x2 = max_corner.x;
//             n  = glm::dvec3(1, 0, 0);
//         } else if (i == 1) {
//             ro = ray_pos.y;
//             rd = ray_dir.y;
//             x1 = min_corner.y;
//             x2 = max_corner.y;
//             n  = glm::dvec3(0, 1, 0);
//         } else if (i == 2) {
//             ro = ray_pos.z;
//             rd = ray_dir.z;
//             x1 = min_corner.z;
//             x2 = max_corner.z;
//             n  = glm::dvec3(0, 0, 1);
//         }

//         // Parallel.
//         if (ifEqual(rd, 0.0)) {
//             if (ro < x1 || ro > x2)
//                 return false;
//         } else {
//             t1 = (x1 - ro) / rd;
//             t2 = (x2 - ro) / rd;

//             if (t1 > t2) {
//                 double swap = t1;
//                 t1          = t2;
//                 t2          = swap;

//                 overturn_normal = true;
//             }

//             if (t1 > t_near) {
//                 t_near     = t1;
//                 hit_normal = overturn_normal ? (-n) : n;
//             }
//             if (t2 < t_far)
//                 t_far = t2;
//         }
//     }
//     // Missing.
//     if (t_near > t_far)
//         return false;
//     // Box is behind.
//     if (t_far < r.getMinT())
//         return false;
//     if (r.isValidT(t_near)) {
//         h.intersection_object       = this;
//         h.intersection_point        = r.pointAtParameter(t_near);
//         h.t                         = t_near;
//         h.intersection_point_normal = hit_normal;
//         r.setMaxT(t_near);
//         return true;
//     } else if (r.isValidT(t_far)) {
//         return false;
//     } else {
//         throw std::runtime_error("Box: intersect wrong!");
//     }

//     return false;
// }

// AABB Box::cal_AABB()
// {
//     glm::dvec3 _min = this->min_corner;
//     glm::dvec3 _max = this->max_corner;
//     return AABB(_min, _max);
// }