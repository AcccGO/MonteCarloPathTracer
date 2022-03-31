#include "mesh_group.h"

double MeshGroup::area()
{
    double result = 0.0;
    for (auto o : this->objects) {
        result += o->area();
    }
    return result;
}

MeshGroup::MeshGroup(string name = "default")
{
    this->mesh_group_name = name;
}

MeshGroup::~MeshGroup()
{
}

bool MeshGroup::intersect(Ray &r, Hit &h)
{
    return true;
}

AABB MeshGroup::cal_AABB()
{
    AABB tmp;
    return tmp;
}