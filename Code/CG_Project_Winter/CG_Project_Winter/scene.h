#pragma once
#include <iostream>
#include <json.hpp>
#include <string>
#include <vector>

#include "emitter.h"
#include "kd_tree.h"
#include "math.h"
#include "mesh_group.h"
#include "model.h"
#include "perspective_camera.h"

using namespace std;
using json = nlohmann::json;

class Object3D;
class Triangle;

class Scene
{
public:
    void loadSceneConfig(string config_file, int choice);
    Scene();
    ~Scene();
    glm::dvec2  getFilmResolution();
    vector<Ray> getRays(double x, double y, int spp, Sampler& sampler);
    bool        intersect(Ray& r, Hit& h);

private:
    void           parseScene(string config_path, int choice);
    nlohmann::json loadJson(const std::string& file_path);
    void           loadObj(string obj_file);
    void           initKdTree();
    double         getShapeArea(const size_t shapeID, Distribution1D& faceAreaDistribution);

public:
    std::string       save_path;
    std::string       scene_name;
    PerspectiveCamera camera;
    vector<Emitter>   emitters;
    glm::dvec3        total_emitters_power         = glm::dvec3(0.0);
    double            total_emitters_average_power = 0.0;
    Distribution1D    emitters_average_power_distribution;
    glm::dvec3        ambient;

    Model* model;

private:
    KdTree* kdtree;
    int     width, height;
};