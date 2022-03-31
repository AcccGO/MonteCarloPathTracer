#include "material.h"

#include "frame.h"

Material::Material()
{
    this->material_name       = "default material";
    this->ambient             = glm::dvec3(0.0);
    this->diffuse_reflection  = glm::dvec3(0.0);
    this->specular_reflection = glm::dvec3(0.0);
    this->shiness             = 0.0;

    this->refraction_index    = 1.0;
    this->transmission_filter = glm::dvec3(1.0);
    this->emission            = glm::dvec3(0.0);

    this->scale         = 1.0;
    this->material_type = MType::None;
}

Material::Material(string name)
{
    this->material_name       = name;
    this->ambient             = glm::dvec3(0.0);
    this->diffuse_reflection  = glm::dvec3(0.0);
    this->specular_reflection = glm::dvec3(0.0);
    this->shiness             = 0.0;

    this->refraction_index    = 1.0;
    this->transmission_filter = glm::dvec3(1.0);
    this->emission            = glm::dvec3(0.0);

    this->scale         = 1.0;
    this->material_type = MType::None;
}

Material::Material(string name, const glm::dvec3& ka,
                   const glm::dvec3& kd,
                   const glm::dvec3& ks,
                   double            ns,
                   double            ni,
                   glm::dvec3        e = glm::dvec3(0.0))
{
    this->material_name = name;

    this->diffuse_reflection  = kd;
    this->ambient             = ka;
    this->specular_reflection = ks;
    this->shiness             = ns;

    this->refraction_index    = ni;
    this->transmission_filter = glm::dvec3(1.0);
    this->emission            = e;

    this->scale         = 1.0;
    this->material_type = MType::None;
}

Material::Material(string name, const glm::dvec3& d_color)
{
    this->material_name       = name;
    this->ambient             = glm::dvec3(0.0);
    this->diffuse_reflection  = d_color;
    this->specular_reflection = glm::dvec3(0.0);
    this->shiness             = 0.0;

    this->refraction_index    = 1.0;
    this->transmission_filter = glm::dvec3(1.0);
    this->emission            = glm::dvec3(0.0);

    this->scale         = 1.0;
    this->material_type = MType::None;
}

Material::~Material()
{
}

glm::dvec3 Material::sample(Hit& hit, Sampler& sampler, double* pdf)
{
    switch (this->material_type) {
    case MType::Diffuse:
        return sampleDiffuse(hit, sampler, pdf);
        break;
    case MType::Glass:
        return sampleGlass(hit, sampler, pdf);
        break;
    case MType::Phong:
        return samplePhong(hit, sampler, pdf);
        break;
    default:
        throw std::runtime_error("Unable to sample this material type\n");
        break;
    }
}

glm::dvec3 Material::sampleDiffuse(Hit& hit, Sampler& sampler, double* pdf)
{
    auto       _sample = sampler.next2D();
    glm::dvec3 val(0.0);
    glm::dvec3 local_wi = cosHemisphere(_sample);
    hit.wi              = local_wi;
    *pdf                = this->pdfDiffuse(hit);
    val                 = this->evalDiffuse(hit);

    return val;
}

glm::dvec3 Material::samplePhong(Hit& hit, Sampler& sampler, double* pdf)
{
    auto       _sample = sampler.next2D();
    glm::dvec3 val(0.0);

    if (_sample.x < specular_sampling_weight) {
        glm::dvec2 new_sample(_sample.x / specular_sampling_weight, _sample.y);

        auto reflection_local_dir = phongReflect(hit.wo);

        Frame reflection_space(reflection_local_dir);

        auto exp = this->shiness;

        auto brdf_sample = phongLobe(new_sample, exp);

        auto brdf_sample_local = reflection_space.toWorld(brdf_sample);

        hit.wi = brdf_sample_local;

        val = evalPhong(hit);
    } else {
        const glm::dvec2 new_sample((_sample.x - specular_sampling_weight) / (1.0 - specular_sampling_weight), _sample.y);

        auto local_wi = cosHemisphere(new_sample);
        hit.wi        = local_wi;
        val           = evalPhong(hit);
    }

    *pdf = pdfPhong(hit);
    return val;
}

glm::dvec3 Material::sampleGlass(Hit& hit, Sampler& sampler, double* pdf)
{
    glm::dvec3 val(1.0);
    *pdf = 1.0;

    bool is_entering = hit.wo.z > 0.0;
    auto eta_i       = 1.0;
    auto eta_t       = this->refraction_index;
    if (!is_entering)
        std::swap(eta_i, eta_t);

    auto eta    = eta_i / eta_t;
    auto sin2_i = std::fmax(0.0, 1.0 - hit.wo.z * hit.wo.z);
    auto sin2_t = eta * eta * sin2_i;
    auto cos_t  = std::sqrtf(std::fmax(0.0, 1.0 - sin2_t));

    cos_t = is_entering ? -cos_t : cos_t;

    auto fresnel = this->fresnelDielectric(eta_i, eta_t, std::fabs(hit.wo.z), std::fabs(cos_t));

    // reflect
    auto _s = sampler.next();
    if (_s < fresnel) {
        hit.wi = phongReflect(hit.wo);
        val    = glm::dvec3(1.0);
    } else {
        hit.wi = glm::dvec3(eta * -hit.wo.x, eta * -hit.wo.y, cos_t);
        val    = this->transmission_filter;
    }

    return val;
}

inline glm::dvec3 Material::phongReflect(const glm::dvec3& d)
{
    return glm::dvec3(-d.x, -d.y, d.z);
}

double Material::pdfGet(const Hit& hit)
{
    switch (this->material_type) {
    case MType::Diffuse:
        return pdfDiffuse(hit);
        break;
    case MType::Glass:
        return pdfGlass(hit);
        break;
    case MType::Phong:
        return pdfPhong(hit);
        break;
    default:
        throw std::runtime_error("Unable to get this material type's pdf\n");
        break;
    }
}

double Material::pdfDiffuse(const Hit& hit)
{
    double pdf = 0.0;
    pdf        = cosHemispherePdf(hit.wi);
    return pdf;
}

double Material::pdfPhong(const Hit& hit)
{
    double pdf = 0.0;

    auto  reflection_local_dir = phongReflect(hit.wo);
    Frame reflection_space(reflection_local_dir);
    auto  brdf_sample = reflection_space.toLocal(hit.wi);

    auto pdf_phong   = phongLobePdf(brdf_sample, this->shiness);
    auto pdf_diffuse = cosHemispherePdf(hit.wi);

    pdf = (pdf_phong * specular_sampling_weight) + (pdf_diffuse * (1 - specular_sampling_weight));

    return pdf;
}

double Material::pdfGlass(const Hit& hit)
{
    double pdf = 0.0;
    return pdf;
}

glm::dvec3 Material::eval(const Hit& hit)
{
    switch (this->material_type) {
    case MType::Diffuse:
        return evalDiffuse(hit);
        break;
    case MType::Glass:
        return evalGlass(hit);
        break;
    case MType::Phong:
        return evalPhong(hit);
        break;
    default:
        throw std::runtime_error("Unable to eval this material type\n");
        break;
    }
}

glm::dvec3 Material::evalDiffuse(const Hit& hit)
{
    glm::dvec3 val(0.0);
    if (Frame::cosTheta(hit.wi) >= 0.0 && Frame::cosTheta(hit.wo) >= 0.0) {
        auto eval_m = this->diffuse_reflection * Frame::cosTheta(hit.wi) * INV_PI;
        return eval_m;
    }
    return val;
}

glm::dvec3 Material::evalPhong(const Hit& hit)
{
    glm::dvec3 val(0.0);
    if (Frame::cosTheta(hit.wi) >= 0.0 && Frame::cosTheta(hit.wo) >= 0.0) {
        val += this->diffuse_reflection * INV_PI;

        auto exp       = this->shiness;
        auto cos_theta = glm::clamp(glm::dot(hit.wi, phongReflect(hit.wo)), 0.0, 1.0);
        val += this->specular_reflection * (exp + 2) * INV_TWOPI * (double)std::powf(cos_theta, exp);
        // auto h = glm::normalize(hit.wi + hit.wo);
        // auto cos_h = glm::clamp(Frame::cosTheta(h), 0.0, 1.0);
        // val += this->specular_reflection*std::pow(cos_h, exp)*INV_TWOPI;

        val *= scale;
        val *= Frame::cosTheta(hit.wi);
    }
    return val;
}

glm::dvec3 Material::evalGlass(const Hit& hit)
{
    glm::dvec3 val(0.0);
    return val;
}

double Material::fresnelDielectric(double eta_i, double eta_t, double cos_i, double cos_t) const
{
    double eta    = eta_i / eta_t;
    double sin2_t = eta * eta * (std::fmax(0.0, 1.0 - cos_i * cos_i));

    if (sin2_t >= 1.0)
        return 1.0;

    double r_parallel = ((eta_t * cos_i) - (eta_i * cos_t)) /
                        ((eta_t * cos_i) + (eta_i * cos_t));
    double r_perpendicular = ((eta_i * cos_i) - (eta_t * cos_t)) /
                             ((eta_i * cos_i) + (eta_t * cos_t));

    return (r_parallel * r_parallel + r_perpendicular * r_perpendicular) * 0.5;
}