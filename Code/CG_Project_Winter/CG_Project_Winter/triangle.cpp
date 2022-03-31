#include "triangle.h"

#include <time.h>

#include "math.h"
#include "mesh_group.h"
using namespace std;

double Triangle::area()
{
    auto temp = glm::cross(ab, ac);
    auto a    = 0.5 * glm::length(temp);
    return fabs(a);
}

glm::dvec3 Triangle::sample() const
{
    double x, y, z;
    do {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        z = (double)rand() / RAND_MAX;
    } while (x + y + z >= 1.0);

    return this->a * x + this->b * y + this->c * z;
}

Triangle::Triangle(vector<glm::dvec3> vertex_from_obj, vector<glm::dvec3> normal_from_obj, vector<glm::dvec2> texture_from_obj, MeshGroup *parent_mesh_group)
{
    this->vertex_from_obj  = vertex_from_obj;
    this->normal_from_obj  = normal_from_obj;
    this->texture_from_obj = texture_from_obj;

    this->a  = vertex_from_obj[0];
    this->b  = vertex_from_obj[1];
    this->c  = vertex_from_obj[2];
    this->na = normal_from_obj[0];
    this->nb = normal_from_obj[1];
    this->nc = normal_from_obj[2];

    this->ab = this->b - this->a;
    this->ac = this->c - this->a;

    this->normal            = glm::normalize(glm::cross(ab, ac));
    this->parent_mesh_group = parent_mesh_group;
}

glm::dvec3 Triangle::baryCoords(const glm::dvec3 &pt)
{
    glm::dvec3 bc;

    // Find the area of the whole triangle.
    glm::dvec3 vi   = ab;
    glm::dvec3 vj   = ac;
    double     area = fabs(glm::dot(glm::cross(vi, vj), this->normal) / 2.0f);
    if (area < 1e-8) {
        return glm::dvec3(0.0);  // Zero-area triangle (points coplanar or coincident).
    }

    // Three vectors radiating from the point to the vertices.
    glm::dvec3 pv0 = a - pt;
    glm::dvec3 pv1 = b - pt;
    glm::dvec3 pv2 = c - pt;

    // Calculate the areas of each sub-triangle.
    double a0 = fabs(glm::dot(glm::cross(pv1, pv2), normal) / 2.0);
    double a1 = fabs(glm::dot(glm::cross(pv2, pv0), normal) / 2.0);
    double a2 = fabs(glm::dot(glm::cross(pv0, pv1), normal) / 2.0);

    bc.x = a0 / area;
    bc.y = a1 / area;
    bc.z = a2 / area;

    return bc;
}

bool Triangle::intersect(Ray &r, Hit &h)
{
    // Refer to realtime rendering p748.
    glm::dvec3 n  = glm::cross(this->ab, this->ac);
    glm::dvec3 s  = r.getOrigin() - this->a;
    glm::dvec3 m  = glm::cross(s, r.getDirection());
    glm::dvec3 d  = r.getDirection();
    glm::dvec3 e1 = ab;
    glm::dvec3 e2 = ac;

    // Parallel.
    double if_p = glm::dot(n, d);
    if (if_p == 0)
        return false;

    double temp = -1.0 * if_p;
    double u    = glm::dot(m, e2) / temp;
    double v    = glm::dot(-m, e1) / temp;
    double t    = glm::dot(n, s) / temp;

    if (u >= 0 && v >= 0 && u + v <= 1 && r.isValidT(t)) {
        r.setMaxT(t);

        h.intersection_object       = this;
        h.t                         = t;
        auto bc                     = baryCoords(h.intersection_point);
        h.intersection_point_normal = glm::normalize(bc.x * na + bc.y * nb + bc.z * nc);

        // Add.
        h.u        = bc.y;
        h.v        = bc.z;
        h.frame_ng = Frame(glm::normalize(glm::cross(ab, ac)));
        h.frame_ns = Frame(h.intersection_point_normal);
        h.wo       = h.frame_ns.toLocal(-r.getDirection());

        h.intersection_point = r.pointAtParameter(t) /* + EPS * h.intersection_point_normal*/;
        return true;
    }

    return false;
}

AABB Triangle::cal_AABB()
{
    if (!has_caled) {
        glm::dvec3 _min, _max;

        _min.x = fmin(a.x, fmin(b.x, c.x));
        _min.y = fmin(a.y, fmin(b.y, c.y));
        _min.z = fmin(a.z, fmin(b.z, c.z));

        _max.x = fmax(a.x, fmax(b.x, c.x));
        _max.y = fmax(a.y, fmax(b.y, c.y));
        _max.z = fmax(a.z, fmax(b.z, c.z));

        this->has_caled    = true;
        this->bounding_box = AABB(_min, _max);
    }

    return this->bounding_box;
}