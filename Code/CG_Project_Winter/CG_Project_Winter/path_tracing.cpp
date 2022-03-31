#include "path_tracing.h"

#include <time.h>

#include <cmath>

#include "frame.h"
#include "math.h"
#include "parallelfor.h"

PathTracing::PathTracing()
{
    bg_color       = glm::dvec3(0.0f);
    tracing_result = NULL;

    rr_depth     = 5;
    rr_prob      = 0.95;
    spp          = 50;
    light_sample = 5;
    bsdf_sample  = 10;
    max_bounce   = 10 /*20*/;
    is_explicit  = true;
}

PathTracing::~PathTracing()
{
    delete scene;
    scene = NULL;

    delete tracing_result;
    tracing_result = NULL;
}

void PathTracing::save_result()
{
    auto e = is_explicit ? "explicit" : "implicit";
    auto p = scene->save_path + "_bounce" + to_string(this->max_bounce) + "_spp" + to_string(this->spp) + "_shadowRay" + to_string(this->light_sample) + "_bsdfRay" + to_string(this->bsdf_sample) + "_rrDepth" + to_string(this->rr_depth) + "_rrProb" + to_string(this->rr_prob) + "_" + e + ".tga";
    this->tracing_result->SaveTGA(p.c_str());
}

void PathTracing::traceScene(string config_file, int choice = 0)
{
    // Init scene.
    if (NULL != scene)
        delete scene;
    scene = new Scene();
    scene->loadSceneConfig(config_file, choice);
    this->scene_resolution = scene->getFilmResolution();

    // Init result image.
    if (NULL != tracing_result)
        delete tracing_result;
    tracing_result = new Image(scene_resolution[0], scene_resolution[1]);

    // Tracing.
    run();

    // Save result.
    save_result();
}

void PathTracing::run()
{
    Sampler sampler = Sampler(260450963);

    bool if_multithread = true;

    int                scene_width  = this->scene_resolution.x;
    int                scene_height = this->scene_resolution.y;
    const unsigned int pixelSize    = scene_width * scene_height;

    parallel_for(
        pixelSize, [&](int start, int end) {
            for (int pixel = start; pixel < end; pixel++) {
                int j = pixel / scene_width;
                int i = pixel % scene_width;

                auto rays  = this->scene->getRays(i, j, spp, sampler);
                auto color = glm::dvec3(0.0);
                for (auto r_index : rays) {
                    auto c = render(r_index, sampler);
                    color += limit(c, 0.0, 1.0);
                }

                auto result = spp == 0 ? glm::dvec3(0.0) : color * (1.0 / spp);
                printf("%d,%d:(%f,%f,%f)\n", i, j, result.x, result.y, result.z);
                this->tracing_result->SetPixel(i, j, result);
            }
        },
        if_multithread);
}

glm::dvec3 PathTracing::render(const Ray& ray, Sampler& sampler)
{
    Ray r = ray;
    Hit hit;
    if (this->scene->intersect(r, hit)) {
        if (is_explicit)
            return this->renderExplicit(r, hit, sampler);
        else
            return this->renderImplicit(r, hit, sampler);
    }
    return glm::dvec3(0.0);
}

glm::dvec3 PathTracing::renderExplicit(const Ray& ray, Hit& hit, Sampler& sampler)
{
    glm::dvec3 li(0.0);
    auto       e = hit.intersection_object->obj_material.emission;
    if (e != glm::dvec3(0.0)) {
        li = e;
    } else {
        li = traceExplicit(hit, 0, sampler);
    }
    return li;
}

glm::dvec3 PathTracing::traceExplicit(Hit& hit, int depth, Sampler& sampler)
{
    glm::dvec3 lr(0.0);
    auto       rr_sample = sampler.next();
    glm::dvec3 indirect_estimator(0.0);
    glm::dvec3 direct_estimator(0.0);
    auto       hit_material = hit.intersection_object->obj_material;

    if (depth < this->max_bounce || (this->max_bounce == -1 && (depth < this->rr_depth || rr_sample < this->rr_prob))) {
        // Indirect illumination.
        {
            int  samples_num = 0;
            auto ind_rr_prob = 0.95;
            auto cum_rr_prob = 1.0;

            Hit    new_hit;
            bool   intersection = false;
            double pdf;

            glm::dvec3 bsdf_cos_theta;

            do {
                bsdf_cos_theta = hit_material.sample(hit, sampler, &pdf);
                auto wi_world  = hit.frame_ns.toWorld(hit.wi);

                Ray new_ray(hit.intersection_point, wi_world);
                new_hit      = Hit();
                intersection = this->scene->intersect(new_ray, new_hit);

                samples_num++;
            } while ((!intersection || new_hit.intersection_object->obj_material.emission != glm::dvec3(0.0)) && sampler.next() < ind_rr_prob);

            cum_rr_prob = samples_num > 1 ? ind_rr_prob : 1.0;

            if (intersection && bsdf_cos_theta != glm::dvec3(0.0)) {
                if (new_hit.intersection_object->obj_material.emission == glm::dvec3(0.0)) {
                    auto li            = traceExplicit(new_hit, depth + 1, sampler);
                    indirect_estimator = li * bsdf_cos_theta /**Frame::cosTheta(hit.wi)*/ * (1.0 / pdf) * (1.0 / samples_num) * (1.0 / cum_rr_prob);
                }
            }
        }

        // Direct illumination.
        {
            glm::dvec3 emitter_estimator(0.0);
            glm::dvec3 bsdf_estimator(0.0);

            // Emitter estimator.
            {
                for (auto i = 0; i < light_sample; i++) {
                    double     emitter_pdf, emitter_area_pdf;
                    glm::dvec3 normal_out, position_out;

                    // Select emitor.
                    auto id = sampler.next() * scene->emitters.size();
                    // auto id = this->scene->emitters_average_power_distribution.sample(sampler.next());
                    id = std::fmin(id, scene->emitters.size() - 1);

                    const Emitter& current_emitter = scene->emitters[id];
                    // emitter_pdf = current_emitter.average_power / scene->total_emitters_average_power;
                    emitter_pdf = 1.0 / this->scene->emitters.size();
                    sampleEmitter(sampler, current_emitter, normal_out, position_out, emitter_area_pdf);

                    auto wi_world               = glm::normalize(position_out - hit.intersection_point);
                    auto wi_local               = hit.frame_ns.toLocal(wi_world);
                    auto light_distance         = glm::length(position_out - hit.intersection_point);
                    auto light_distance_squared = light_distance * light_distance;
                    auto cos_outgoing           = glm::dot(-wi_world, normal_out);

                    if (cos_outgoing > 0.0 && Frame::cosTheta(wi_local) > 0.0) {
                        auto di_ray = Ray(hit.intersection_point, wi_world);
                        di_ray.setMaxT(light_distance);
                        Hit di_hit;
                        if (!scene->intersect(di_ray, di_hit) || di_hit.intersection_object->shapeID == current_emitter.shapeID) {
                            hit.wi                  = wi_local;
                            auto area_to_solidangle = cos_outgoing / (light_distance_squared);
                            auto area_in_solidangle = /*glm::dot(glm::normalize(hit.frame_ns.mz), wi_world) **/ Frame::cosTheta(hit.wi);
                            auto geo_factor         = area_to_solidangle /* * area_in_solidangle*/;
                            auto e                  = current_emitter.radiance;
                            auto bsdf_pdf           = hit_material.pdfGet(hit);

                            auto f      = light_sample * (emitter_area_pdf * emitter_pdf * (1.0 / area_to_solidangle));
                            auto g      = bsdf_sample * bsdf_pdf;
                            auto weight = f / (f + g);

                            auto eval_m = hit_material.eval(hit);
                            emitter_estimator += weight * e * eval_m * (1.0 / emitter_area_pdf) * (1.0 / emitter_pdf) * geo_factor;
                            // emitter_estimator += weight * scene->total_emitters_power * eval_m*geo_factor / PI;
                        }
                    }
                }
                emitter_estimator = this->light_sample == 0 ? glm::dvec3(0.0) : (emitter_estimator / (1.0 * this->light_sample));
            }

            // Bsdf_estimator.
            {
                for (auto i = 0; i < this->bsdf_sample; i++) {
                    // auto sample = sampler.next2D();
                    double bsdf_pdf;
                    auto   bsdf_cos_theta = hit_material.sample(hit, sampler, &bsdf_pdf);
                    if (bsdf_cos_theta != glm::dvec3(0.0)) {
                        Hit        new_hit;
                        glm::dvec3 wi_world   = hit.frame_ns.toWorld(hit.wi);
                        auto       new_ray    = Ray(hit.intersection_point, wi_world);
                        bool       intersects = this->scene->intersect(new_ray, new_hit);
                        if (intersects) {
                            auto li = new_hit.intersection_object->obj_material.emission;
                            if (li == glm::dvec3(0.0)) {
                                continue;
                            }

                            const Emitter& emitter = this->scene->emitters[this->scene->model->all_mesh_groups[new_hit.intersection_object->shapeID].emitterID];
                            // auto emitter_pdf = emitter.average_power / this->scene->total_emitters_average_power;
                            auto emitter_pdf      = 1.0 / this->scene->emitters.size();
                            auto emitter_area_pdf = 1.0 / emitter.area;

                            auto light_distance         = glm::length(new_hit.intersection_point - hit.intersection_point);
                            auto light_distance_squared = light_distance * light_distance;

                            auto cos_out_going = glm::dot(-wi_world, new_hit.frame_ng.mz);

                            if (cos_out_going > 0.0) {
                                auto area_to_solidangle = cos_out_going / (light_distance_squared);
                                auto area_in_solidangle = Frame::cosTheta(hit.wi);
                                auto geo_factor         = area_to_solidangle /** area_in_solidangle*/;

                                auto f      = this->bsdf_sample * bsdf_pdf;
                                auto g      = this->light_sample * emitter_pdf * emitter_area_pdf / area_to_solidangle;
                                auto weight = f / (f + g);

                                bsdf_estimator += weight * li * bsdf_cos_theta / bsdf_pdf;
                            }
                        }
                    }
                }

                bsdf_estimator = (bsdf_sample == 0 ? glm::dvec3(0.0) : (bsdf_estimator / (1.0 * bsdf_sample)));
            }

            direct_estimator = emitter_estimator + bsdf_estimator;
        }

        lr = direct_estimator + indirect_estimator;
        if (max_bounce == -1 && !(depth < rr_depth) && (rr_sample < rr_prob)) {
            lr *= 1.0 / rr_prob;
        }
    }

    return lr;
}

glm::dvec3 PathTracing::renderImplicit(const Ray& ray, Hit& hit, Sampler& sampler)
{
    glm::dvec3 li(0.0);
    auto       e = hit.intersection_object->obj_material.emission;
    if (e != glm::dvec3(0.0)) {
        return e;
    } else {
        li = traceImplicit(hit, 0, sampler);
    }
    return li;
}

glm::dvec3 PathTracing::traceImplicit(Hit& hit, int depth, Sampler& sampler)
{
    if (depth < max_bounce) {
        auto   hit_material = hit.intersection_object->obj_material;
        double pdf;
        auto   bsdf_cos_theta = hit_material.sample(hit, sampler, &pdf);

        Hit  new_hit;
        auto wi_world = hit.frame_ns.toWorld(hit.wi);
        Ray  new_ray(hit.intersection_point, wi_world);
        if (this->scene->intersect(new_ray, new_hit)) {
            if (new_hit.intersection_object->obj_material.emission == glm::dvec3(0.0)) {
                auto li = traceImplicit(new_hit, depth + 1, sampler);
                return li * bsdf_cos_theta * (1.0 / pdf);
            } else {
                auto li = new_hit.intersection_object->obj_material.emission;
                if (glm::dot(new_hit.frame_ns.mz, -wi_world) <= 0.0)
                    li = glm::dvec3(0.0);
                return li * bsdf_cos_theta * (1.0 / pdf);
            }
        } else {
            return glm::dvec3(0.0);
        }
    }
    return glm::dvec3(0.0);
}

void PathTracing::sampleEmitter(Sampler& sampler, const Emitter& emitter, glm::dvec3& n, glm::dvec3& pos, double& pdf) const
{
    auto  mesh_id = emitter.faceAreaDistribution.sample(sampler.next());
    auto& tri     = this->scene->model->all_mesh_groups[emitter.shapeID].objects[mesh_id];
    auto  uv      = uniformTriangle(sampler.next2D());

    pos = barycentric(tri->a, tri->b, tri->c, uv.x, uv.y);
    n   = barycentric(tri->na, tri->nb, tri->nc, uv.x, uv.y);

    pdf = 1.0 / emitter.area;
}
