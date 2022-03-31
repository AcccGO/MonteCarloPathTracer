//**************************************************************************************
//  Copyright (C) 2019 - 2022, Min Tang
//  All rights reserved.
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//     1. Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//     2. Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//     3. The names of its contributors may not be used to endorse or promote
//        products derived from this software without specific prior written
//        permission.
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//**************************************************************************************

#pragma once
#include <float.h>
#include <stdlib.h>

#include "hit.h"
#include "math.h"

class AABB
{
    void init()
    {
        _max = glm::dvec3(MIN_INFINITY_NEGATIVE, MIN_INFINITY_NEGATIVE, MIN_INFINITY_NEGATIVE);

        _min = glm::dvec3(MAX_INFINITY_POSITIVE, MAX_INFINITY_POSITIVE, MAX_INFINITY_POSITIVE);
    }

public:
    glm::dvec3 _min;
    glm::dvec3 _max;

    double t;  // Hit with ray.

    AABB()
    {
        init();
    }

    AABB(const glm::dvec3 &v)
    {
        _min = _max = v;
    }

    AABB(const glm::dvec3 &a, const glm::dvec3 &b)
    {
        _min = a;
        _max = a;
        vmin(_min, b);
        vmax(_max, b);
    }

    AABB(const AABB &aabb)
    {
        _min = aabb._min;
        _max = aabb._max;
    }

    void vmin(glm::dvec3 &a, const glm::dvec3 &b)
    {
        a.x = fmin(a[0], b[0]);
        a.y = fmin(a[1], b[1]);
        a.z = fmin(a[2], b[2]);
    }

    void vmax(glm::dvec3 &a, const glm::dvec3 &b)
    {
        a.x = fmax(a[0], b[0]);
        a.y = fmax(a[1], b[1]);
        a.z = fmax(a[2], b[2]);
    }

    bool overlaps(const AABB &b) const
    {
        if (_min[0] > b._max[0]) return false;
        if (_min[1] > b._max[1]) return false;
        if (_min[2] > b._max[2]) return false;

        if (_max[0] < b._min[0]) return false;
        if (_max[1] < b._min[1]) return false;
        if (_max[2] < b._min[2]) return false;

        return true;
    }

    bool overlaps(const AABB &b, AABB &ret) const
    {
        if (!overlaps(b))
            return false;

        ret._min = glm::dvec3(
            fmax(_min[0], b._min[0]),
            fmax(_min[1], b._min[1]),
            fmax(_min[2], b._min[2]));

        ret._max = glm::dvec3(
            fmin(_max[0], b._max[0]),
            fmin(_max[1], b._max[1]),
            fmin(_max[2], b._max[2]));

        return true;
    }

    bool inside(const glm::dvec3 &p) const
    {
        if (p[0] < _min[0] || p[0] > _max[0]) return false;
        if (p[1] < _min[1] || p[1] > _max[1]) return false;
        if (p[2] < _min[2] || p[2] > _max[2]) return false;

        return true;
    }

    AABB &operator+=(const glm::dvec3 &p)
    {
        vmin(_min, p);
        vmax(_max, p);
        return *this;
    }

    AABB &operator+=(const AABB &b)
    {
        vmin(_min, b._min);
        vmax(_max, b._max);
        return *this;
    }

    AABB operator+(const AABB &v) const
    {
        AABB rt(*this);
        return rt += v;
    }

    double width() const
    {
        return _max[0] - _min[0];
    }

    double height() const
    {
        return _max[1] - _min[1];
    }

    double depth() const
    {
        return _max[2] - _min[2];
    }

    glm::dvec3 center() const
    {
        return (_min + _max) * 0.5;
    }

    double volume() const
    {
        return width() * height() * depth();
    }

    bool empty() const
    {
        return _max.x < _min.x;
    }

    void enlarge(double thickness)
    {
        _max += glm::dvec3(thickness, thickness, thickness);
        _min -= glm::dvec3(thickness, thickness, thickness);
    }

    glm::dvec3 getMax() { return _max; }
    glm::dvec3 getMin() { return _min; }

    void print(FILE *fp)
    {
        fprintf(fp, "%lf, %lf, %lf, %lf, %lf, %lf\n", _min.x, _min.y, _min.z, _max.x, _max.y, _max.z);
    }

    bool intersectAABB(Ray &r, Hit &h)
    {
        auto o = r.getOrigin();
        if (o.x > _min.x && o.y > _min.y && o.z > _min.z && o.x < _max.x && o.y < _max.y && o.z < _max.z) {
            return true;
        }

        glm::dvec3 bbox[2] = {_min, _max};
        auto       dir     = r.getDirection();
        int        xsign   = (int)(dir.x < 0.0);
        double     invdirx = 1.0 / dir.x;
        double     tmin    = (bbox[xsign].x - o.x) * invdirx;
        double     tmax    = (bbox[1 - xsign].x - o.x) * invdirx;

        int    ysign   = (int)(dir.y < 0.0);
        double invdiry = 1.0 / dir.y;
        double tymin   = (bbox[ysign].y - o.y) * invdiry;
        double tymax   = (bbox[1 - ysign].y - o.y) * invdiry;

        if ((tmin > tymax) || (tymin > tmax)) {
            return false;
        }

        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        int    zsign   = (int)(dir.z < 0.0);
        double invdirz = 1.0 / dir.z;
        double tzmin   = (bbox[zsign].z - o.z) * invdirz;
        double tzmax   = (bbox[1 - zsign].z - o.z) * invdirz;

        if ((tmin > tzmax) || (tzmin > tmax)) {
            return false;
        }

        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;

        return (tmin < r.getMaxT()) && (tmax > r.getMinT());
    }
};