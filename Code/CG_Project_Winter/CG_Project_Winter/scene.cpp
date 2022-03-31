#include "scene.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "object3d.h"
#include "parse_string.h"

using namespace std;
using json = nlohmann::json;

Scene::Scene()
{
    total_emitters_power         = glm::dvec3(0.0);
    total_emitters_average_power = 0.0;
    ambient                      = glm::dvec3(0.0);
    model                        = NULL;
    kdtree                       = NULL;
}

Scene::~Scene()
{
    delete model;
    model = NULL;

    delete kdtree;
    kdtree = NULL;
}

glm::dvec2 Scene::getFilmResolution()
{
    return glm::dvec2(this->camera.width, this->camera.height);
}

vector<Ray> Scene::getRays(double x, double y, int spp, Sampler& sampler)
{
    vector<Ray> rays;

    for (auto i = 0; i < spp; i++) {
        auto random_sample = sampler.next2D();
        // auto rx = random_sample.x - 0.5;
        // auto ry = random_sample.y - 0.5;
        auto rand_x = (x + random_sample.x) / (1.0f * this->width);
        auto rand_y = (y + random_sample.y) / (1.0f * this->height);
        // cout << "pixel(" << x << "," << y << ")'s random offset is (" << rand_x << "," << rand_y << ")" << endl;
        // auto rand_ray_pos = glm::dvec2(rand_x, rand_y);
        rays.push_back(this->camera.generateRay(glm::dvec2(rand_x, rand_y)));
    }

    return rays;
}

bool Scene::intersect(Ray& r, Hit& h)
{
    if (NULL == this->kdtree)
        this->initKdTree();
    return this->kdtree->intersect(r, h);
}

void Scene::loadSceneConfig(string config_file, int choice = 0)
{
    this->parseScene(config_file, choice);
    this->initKdTree();
}

void Scene::initKdTree()
{
    if (NULL != this->kdtree)
        delete this->kdtree;

    this->kdtree = new KdTree(this->model->all_objs);
    this->kdtree->root_node->updateBbox();
    int split_depth = round(log10((double)model->all_objs.size()) * 1);
    this->kdtree->split(4 * split_depth /*25*/);

    string p = scene_name + "_KdTree.txt";
    // this->kdtree->writeKDtoFile(this->kdtree->root_node, p.c_str());
}

void Scene::parseScene(string config_path, int choice = 0)
{
    auto scene_json = loadJson(config_path).at("scene");

    auto index = 0;

    if (scene_json.is_array()) {
        for (json::iterator it = scene_json.begin(); it != scene_json.end(); ++it, ++index) {
            if (index == choice) {
                auto target_scene = it.value();

                // Set result saved path.
                this->scene_name = target_scene["name"].get<std::string>();
                this->save_path  = target_scene["root_path"].get<std::string>() + target_scene["name"].get<std::string>();

                // Load obj.
                auto obj_path = target_scene["root_path"].get<std::string>() + target_scene["obj_path"].get<std::string>();
                this->loadObj(obj_path);

                // Set camera.
                auto   camera_config   = target_scene["camera"];
                auto   position        = glm::dvec3(camera_config["position"][0], camera_config["position"][1], camera_config["position"][2]);
                auto   look_at         = glm::dvec3(camera_config["look-at"][0], camera_config["look-at"][1], camera_config["look-at"][2]);
                auto   up              = glm::dvec3(camera_config["up"][0], camera_config["up"][1], camera_config["up"][2]);
                double fov             = camera_config["fov"].get<double>();
                auto   film_resolution = glm::dvec2(camera_config["film-resolution"][0], camera_config["film-resolution"][1]);
                this->camera           = PerspectiveCamera(position, look_at, up, fov, film_resolution);

                // Set film_resolution.
                this->width  = film_resolution[0];
                this->height = film_resolution[1];
            }
        }
    }
}

nlohmann::json Scene::loadJson(const std::string& file_path)
{
    nlohmann::json json_data;

    std::ifstream input(file_path);

    if (input.fail()) {
        throw std::runtime_error("Unable to open file " + file_path);
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string contents(buffer.str());
    // cout << contents << endl;

    json_data = json::parse(contents.c_str());

    // cout << json_data.dump(4) << endl;
    // cout << "++++++++++++++++++++++++++++++++/n";

    return json_data;
}

void Scene::loadObj(string obj_file)
{
    if (this->model != NULL)
        delete model;

    model = new Model(obj_file);

    //��ɢmesh group
    model->splitMeshGroup();

    //����model��ȡ�ĵƹ�
    size_t g_index = 0;
    size_t l_index = 0;
    for (auto& g : model->all_mesh_groups) {
        if (g.obj_material.emission != glm::dvec3(0.0)) {
            Distribution1D faceAreaDistribution;

            double shape_area    = getShapeArea(g_index, faceAreaDistribution);
            auto   power         = PI * shape_area * g.obj_material.emission;
            auto   average_power = 1.0 / 3 * (power.x + power.y + power.z);

            emitters.emplace_back(Emitter{g_index, shape_area, g.obj_material.emission, faceAreaDistribution, average_power});

            g.emitterID = l_index;
            l_index++;

            this->total_emitters_average_power += average_power;
            this->total_emitters_power += power;
            emitters_average_power_distribution.add(average_power);
        }
        g_index++;
    }
    emitters_average_power_distribution.normalize();

    //��ɢmesh group
    model->splitMeshGroup();

    // add sphere
    // model->addSphere(glm::dvec3(200.0,200.0,200.0), 50.0);
}

double Scene::getShapeArea(const size_t shapeID, Distribution1D& faceAreaDistribution)
{
    auto g = this->model->all_mesh_groups[shapeID];
    for (size_t i = 0; i < g.objects.size(); i++) {
        auto area = g.objects[i]->area();
        faceAreaDistribution.add(area);
    }

    const auto result = faceAreaDistribution.cdf.back();
    faceAreaDistribution.normalize();
    return result;
}