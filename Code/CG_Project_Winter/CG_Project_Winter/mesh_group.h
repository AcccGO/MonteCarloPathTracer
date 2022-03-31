#pragma once
#include <vector>

#include "object3d.h"
#include "sphere.h"
#include "triangle.h"

class MeshGroup : public Object3D
{
public:
    virtual bool intersect(Ray &r, Hit &h);
    virtual AABB cal_AABB();
    double       area();

    MeshGroup(string name);
    ~MeshGroup();

public:
    std::vector<Triangle *> objects;
    string                  mesh_group_name;
    int                     emitterID = -1;
};