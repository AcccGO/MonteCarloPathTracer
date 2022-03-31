#pragma once
#pragma once
#include <vector>

#include "image.h"
#include "scene.h"
class PathTracing
{
public:
    PathTracing();
    ~PathTracing();
    void traceScene(string config_file, int choice);
    void save_result();

private:
    void       run();
    glm::dvec3 render(const Ray& ray, Sampler& sampler);
    glm::dvec3 renderExplicit(const Ray& ray, Hit& hit, Sampler& sampler);
    glm::dvec3 traceExplicit(Hit& hit, int depth, Sampler& sampler);
    glm::dvec3 renderImplicit(const Ray& ray, Hit& hit, Sampler& sampler);
    glm::dvec3 traceImplicit(Hit& hit, int depth, Sampler& sampler);
    void       sampleEmitter(Sampler& sampler, const Emitter& emitter, glm::dvec3& n, glm::dvec3& pos, double& pdf) const;

private:
    Scene*     scene = NULL;
    glm::dvec2 scene_resolution;
    glm::dvec3 bg_color;

    int    rr_depth;
    double rr_prob;
    int    spp;
    int    light_sample;
    int    bsdf_sample;
    int    max_bounce;
    bool   is_explicit;

    Image* tracing_result = NULL;
};