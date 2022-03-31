#pragma once
#include <common.hpp>
#include <geometric.hpp>
#include <glm.hpp>
#include <map>
#include <string>
#include <vec3.hpp>
#include <vector>

#include "emitter.h"
#include "mesh_group.h"
#include "parse_string.h"

using namespace std;
using namespace glm;

class Model
{
public:
    vector<MeshGroup> all_mesh_groups;
    vector<Triangle*> all_objs;

    vector<glm::dvec3> obj_normals;
    vector<glm::dvec3> obj_vertices;
    vector<glm::dvec2> obj_texture_indexes;

    map<string, Material> material_mapping;

    string obj_path, mtl_path;

public:
    Model(string obj_path);
    void parseObj(string obj_path);
    void parseMaterial(string mtl_path);
    void generateBSDF();

    MeshGroup* creatNewMeshGroup(MeshGroup* c, string new_name);

    void splitMeshGroup();

    void addSphere(glm::dvec3 pos, double r);
};