#pragma once

#include <glm.hpp>
#include <mat4x4.hpp>
#include <vec3.hpp>

#include "AABB.h"
#include "hit.h"
#include "material.h"

class Object3D
{
public:
    Object3D();

    virtual bool   intersect(Ray &r, Hit &h) = 0;
    virtual AABB   cal_AABB()                = 0;
    virtual double area()                    = 0;

public:
    Material obj_material;
};