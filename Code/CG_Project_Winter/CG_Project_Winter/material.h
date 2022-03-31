#pragma once

#include <Windows.h>

#include <glm.hpp>
#include <string>
#include <vec3.hpp>
#include <vector>

#include "hit.h"
#include "math.h"
using namespace std;

enum MType {
    Phong,
    Glass,
    Diffuse,
    None,
};

class Material
{
public:
    Material(string name);
    Material(string            name,
             const glm::dvec3& d_color);
    Material(string            name,
             const glm::dvec3& ka,
             const glm::dvec3& kd,
             const glm::dvec3& ks,
             double            ns,
             double            ni,
             glm::dvec3        e);
    Material();
    ~Material();

public:
    glm::dvec3 sample(Hit& hit, Sampler& sampler, double* pdf);
    glm::dvec3 sampleDiffuse(Hit& hit, Sampler& sampler, double* pdf);
    glm::dvec3 samplePhong(Hit& hit, Sampler& sampler, double* pdf);
    glm::dvec3 sampleGlass(Hit& hit, Sampler& sampler, double* pdf);

    double pdfGet(const Hit& hit);
    double pdfDiffuse(const Hit& hit);
    double pdfPhong(const Hit& hit);
    double pdfGlass(const Hit& hit);

    glm::dvec3 eval(const Hit& hit);
    glm::dvec3 evalDiffuse(const Hit& hit);
    glm::dvec3 evalPhong(const Hit& hit);
    glm::dvec3 evalGlass(const Hit& hit);

public:
    inline glm::dvec3 phongReflect(const glm::dvec3& d);
    double            fresnelDielectric(double eta_i, double eta_t, double cos_i, double cos_t) const;

public:
    std::string material_name;

    glm::dvec3 ambient;              // ka
    glm::dvec3 diffuse_reflection;   // kd
    glm::dvec3 specular_reflection;  // ks
    double     shiness;              // Ns

    double     refraction_index;     // Ni
    glm::dvec3 transmission_filter;  // Tf

    glm::dvec3 emission;  // light ka

    double       scale;
    double       specular_sampling_weight = 0.0;
    unsigned int material_type            = 0;
};
