#pragma once
#include <float.h>

#include <cmath>
#include <glm.hpp>
#include <random>

#define PI         3.14159265358979323846
#define INV_PI     0.31830988618379067154
#define INV_TWOPI  0.15915494309189533577
#define INV_FOURPI 0.07957747154594766788

// FLT_EPSILON
#define EPS                   1e-5              // In ray tracing, do NOT report intersection for rays starting at the surface(no false positive).
#define MAX_INFINITY_POSITIVE 3.402823466e+38F  // FLT_MAX in float.h.
#define MIN_INFINITY_NEGATIVE -3.402823466e+38F

inline bool ifEqual(double a, double b)
{
    if (fabs(a - b) < EPS)
        return true;

    return false;
}

inline int mCompare(double a, double b)
{
    if (ifEqual(a, b))
        return 0;

    if (a < b)
        return -1;

    return 1;
}

inline double safeSqrt(double v)
{
    return std::sqrt(std::fmax(0.0, v));
}

inline glm::dvec3 limit(const glm::dvec3& src, double bottom, double top)
{
    assert(top >= bottom);
    glm::dvec3 result = src;

    result.x = std::fmax(bottom, src.x);
    result.y = std::fmax(bottom, src.y);
    result.z = std::fmax(bottom, src.z);

    result.x = std::fmin(top, src.x);
    result.y = std::fmin(top, src.y);
    result.z = std::fmin(top, src.z);
    return result;

    /*auto max = std::fmax(src.x, std::fmax(src.y, src.z));
    if (max > top)
    {
        result.x = top / max * src.x;
        result.y = top / max * src.y;
        result.z = top / max * src.z;
    }
    return result;*/
}

// Pseudo - random sampler(Mersenne Twister 19937) structure.
struct Sampler {
    std::mt19937                           g;
    std::uniform_real_distribution<double> d;

    explicit Sampler(int seed)
    {
        g = std::mt19937(seed);
        d = std::uniform_real_distribution<double>(0.0, 1.0);
    }

    double next()
    {
        return d(g);
    }

    glm::dvec2 next2D()
    {
        return {d(g), d(g)};
    }

    void setSeed(int seed)
    {
        g.seed(seed);
        d.reset();
    }
};

template <class T>
inline T barycentric(const T& a, const T& b, const T& c, const double u, const double v)
{
    return a * (1 - u - v) + b * u + c * v;
}

template <class T>
inline T mclamp(T v, T m_min, T m_max)
{
    return std::min(std::max(v, m_min), m_max);
}

inline bool isZero(const glm::dvec3 v)
{
    return glm::dot(v, v) < EPS;
}

inline void coordinateSystem(const glm::dvec3& a, glm::dvec3& b, glm::dvec3& c)
{
    if (std::fabs(a.x) > std::fabs(a.y)) {
        auto inverse_length = 1.0 / std::sqrt(a.x * a.x + a.z * a.z);
        c                   = glm::dvec3(a.z * inverse_length, 0.0, -a.x * inverse_length);
    } else {
        auto inverse_length = 1.0 / std::sqrt(a.y * a.y + a.z * a.z);
        c                   = glm::dvec3(0.0, a.z * inverse_length, -a.y * inverse_length);
    }
    b = glm::cross(c, a);
}

inline double getLuminance(const glm::dvec3& rgb)
{
    return glm::dot(rgb, glm::dvec3(0.212671, 0.715160, 0.072169));
}

struct Distribution1D {
    std::vector<double> cdf{0};
    bool                is_normalized = false;

    inline void add(double pdf)
    {
        cdf.push_back(cdf.back() + pdf);
    }

    unsigned int size()
    {
        return cdf.size() - 1;
    }

    double normalize()
    {
        auto sum = cdf.back();
        for (auto& v : cdf) {
            v /= sum;
        }
        is_normalized = true;
        return sum;
    }

    inline double pdf(unsigned int i) const
    {
        assert(is_normalized);
        return cdf[i + 1] - cdf[i];
    }

    int sample(float sample) const
    {
        assert(is_normalized);
        const auto it = std::upper_bound(cdf.begin(), cdf.end(), sample);
        return mclamp(int(std::distance(cdf.begin(), it)) - 1, 0, int(cdf.size()) - 2);
    }
};

inline glm::dvec3 uniformSphere(const glm::dvec2& sample)
{
    glm::dvec3 v(0.0);
    double     phi       = sample.x * PI * 2.0f;
    double     cos_theta = 1.0 - (2.0 * sample.y);
    double     sin_theta = std::sqrt(std::max(1.0 - cos_theta * cos_theta, 0.0));
    v                    = glm::dvec3(sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta);
    return v;
}

inline double uniformSpherePdf()
{
    double pdf = 0.0;
    pdf        = INV_FOURPI;
    return pdf;
}

inline glm::dvec3 uniformHemisphere(const glm::dvec2& sample)
{
    glm::dvec3 v(0.0);
    double     phi       = sample.x * PI * 2.0f;
    double     cos_theta = sample.y;
    double     sin_theta = std::sqrt(std::max(1.0 - (cos_theta * cos_theta), 0.0));
    v                    = glm::dvec3(sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta);
    return v;
}

inline double uniformHemispherePdf(const glm::dvec3& v)
{
    double pdf = 0.0;
    pdf        = INV_TWOPI;
    return pdf;
}

inline glm::dvec2 uniformDiskConcentric(const glm::dvec2& sample)
{
    glm::dvec2 v(0.0);
    double     phi, radius;
    double     remapped_x = (2.0 * sample.x) - 1.0;
    double     remapped_y = (2.0 * sample.y) - 1.0;

    if (remapped_x == 0 && remapped_y == 0) {
        return glm::dvec2(0.0, 0.0);
    }

    if ((remapped_x * remapped_x) > (remapped_y * remapped_y)) {
        radius = remapped_x;
        phi    = (PI * 0.25) * (remapped_y * (1.0 / remapped_x));
    } else {
        radius = remapped_y;
        phi    = (PI * 0.5) - ((PI * 0.25) * (remapped_x * (1.0 / remapped_y)));
    }

    v = glm::dvec2(radius * std::cos(phi), radius * std::sin(phi));
    return v;
}

inline glm::dvec3 cosHemisphere(const glm::dvec2& sample)
{
    glm::dvec3 v(0.0);
    glm::dvec2 disc_sample = uniformDiskConcentric(sample);
    auto       z           = 1.0 - glm::length(disc_sample);
    z                      = std::max(z, 0.0);
    z                      = std::sqrt(z);

    v = glm::dvec3(disc_sample.x, disc_sample.y, z);
    return v;
}

inline double cosHemispherePdf(const glm::dvec3& v)
{
    auto pdf = 0.0;
    if (v.z >= 0.0) {
        pdf = v.z * INV_PI;
    } else {
        pdf = 0.0;
    }
    return pdf;
}

inline glm::dvec3 phongLobe(const glm::dvec2& sample, const double& exponent)
{
    glm::dvec3 v(0.0);
    auto       cos_theta = std::pow(sample.x, 1.0 / (exponent + 1));
    auto       sin_theta = std::sqrt(std::fmax(1.0 - (cos_theta * cos_theta), 0));
    auto       phi       = sample.y * 2.0 * PI;

    v = glm::dvec3(sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta);
    return v;
}

inline double phongLobePdf(const glm::dvec3& v, const double& exponent)
{
    auto pdf = 0.0;
    pdf      = (v.z >= 0.0) ? (exponent + 2) * INV_TWOPI * std::pow(v.z, exponent) : 0.0;

    return pdf;
}

inline glm::dvec2 uniformTriangle(const glm::dvec2& sample)
{
    glm::dvec2 v(0.0);
    auto       u = std::sqrt(1.0 - sample.x);
    v            = {1 - u, u * sample.y};

    return v;
}
