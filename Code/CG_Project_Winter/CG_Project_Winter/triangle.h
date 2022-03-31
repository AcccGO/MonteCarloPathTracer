#pragma once

#include <vector>

#include "object3d.h"

using namespace std;

class MeshGroup;
class Triangle : public Object3D
{
public:
    Triangle(vector<glm::dvec3> vertex_from_obj, vector<glm::dvec3> normal_from_obj, vector<glm::dvec2> texture_from_obj, MeshGroup *parent_mesh_group);

    virtual bool intersect(Ray &r, Hit &h);
    virtual AABB cal_AABB();
    double       area();
    glm::dvec3   sample() const;
    glm::dvec3   baryCoords(const glm::dvec3 &pt);

public:
    glm::dvec3 a;
    glm::dvec3 b;
    glm::dvec3 c;
    glm::dvec3 na;
    glm::dvec3 nb;
    glm::dvec3 nc;

    glm::dvec3 ab;
    glm::dvec3 ac;

    glm::dvec3 normal;

    // Read from obj_file.
    vector<glm::dvec3> vertex_from_obj;
    vector<glm::dvec3> normal_from_obj;
    vector<glm::dvec2> texture_from_obj;

    MeshGroup *parent_mesh_group = NULL;

    AABB bounding_box;
    bool has_caled = false;

    size_t shapeID;
};