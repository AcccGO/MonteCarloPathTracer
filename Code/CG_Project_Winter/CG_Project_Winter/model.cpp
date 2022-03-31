#include "model.h"

#include <time.h>

#include <fstream>
#include <gtc/matrix_transform.hpp>
#include <iostream>

#include "parse_string.h"

using namespace std;

Model::Model(string obj_path)
{
    this->obj_path = obj_path;

    parseObj(obj_path);
    int test;
}

MeshGroup* Model::creatNewMeshGroup(MeshGroup* c, string new_name = "default")
{
    if (c != NULL && c->objects.empty()) {
        all_mesh_groups.pop_back();
    }
    this->all_mesh_groups.emplace_back(MeshGroup(new_name));
    return &all_mesh_groups.back();
}

void Model::parseObj(string obj_path)
{
    ifstream fin(obj_path);
    if (!fin.is_open()) {
        throw std::runtime_error("Unable to find obj file in " + obj_path);
    }
    string         fileString((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    vector<string> lines = Split(fileString, "\n");

    MeshGroup*              current_mesh_group = NULL;
    Material                current_mtl;
    string                  line;
    ParseString             obj_line;
    const unsigned long int line_size  = lines.size();
    unsigned long int       line_index = 0;

    while (line_index < line_size) {
        line = lines[line_index++];
        obj_line.init(line);
        string command = obj_line.GetWord();
        if (command.size() == 0)
            continue;

        // Comment.
        if (command == "#") {
            continue;
        }
        // Mesh group.
        else if (command == "o" || command == "g") {
            current_mesh_group = creatNewMeshGroup(current_mesh_group, obj_line.GetWord());

            continue;
        } else if (command == "mtllib") {
            string mtl_file    = obj_line.GetWord();
            string parent_path = this->obj_path.substr(0, this->obj_path.find_last_of('/') + 1);
            this->mtl_path     = parent_path + mtl_file;

            parseMaterial(this->mtl_path);
        }
        // v
        else if (command == "v") {
            double x = obj_line.GetFloat();
            double y = obj_line.GetFloat();
            double z = obj_line.GetFloat();
            this->obj_vertices.emplace_back(glm::dvec3(x, y, z));

            continue;
        }
        // vn
        else if (command == "vn") {
            double x = obj_line.GetFloat();
            double y = obj_line.GetFloat();
            double z = obj_line.GetFloat();
            this->obj_normals.emplace_back(glm::dvec3(x, y, z));

            continue;
        }
        // vt
        else if (command == "vt") {
            double x = obj_line.GetFloat();
            double y = obj_line.GetFloat();
            this->obj_texture_indexes.emplace_back(glm::dvec2(x, y));

            continue;
        }
        // usemtl
        else if (command == "usemtl") {
            string mtl = obj_line.GetWord();
            if (this->material_mapping.find(mtl) != this->material_mapping.end()) {
                current_mtl                      = this->material_mapping.at(mtl);
                current_mesh_group->obj_material = current_mtl;
            } else {
                throw std::runtime_error("Unable to find mtl " + mtl + " in " + obj_path);
            }

            continue;
        }
        // faces
        else if (command == "f") {
            if (current_mesh_group == NULL) {
                current_mesh_group = creatNewMeshGroup(current_mesh_group);
            }

            glm::vec4 vertex_index, normal_index, texture_index;

            unsigned long int index = 0;
            while (true) {
                string word = obj_line.GetWord();
                if (word == "")
                    break;
                vector<string> subWords = Split(word, "/");
                // v/vt/vn
                if (subWords.size() >= 2) {
                    // v
                    int vi              = atoi(subWords[0].c_str()) - 1;
                    vertex_index[index] = vi;

                    // vn
                    int ni              = atoi(subWords[subWords.size() - 1].c_str()) - 1;
                    normal_index[index] = ni;

                    if (subWords.size() == 3) {
                        // vt
                        int ti               = atoi(subWords[1].c_str()) - 1;
                        texture_index[index] = ti;
                    } else {
                        texture_index[index] = -1;
                    }
                } else if (subWords.size() == 1) {
                    int vi               = atoi(subWords[0].c_str()) - 1;
                    vertex_index[index]  = vi;
                    normal_index[index]  = -1;
                    texture_index[index] = -1;
                } else {
                    throw std::runtime_error("Unable to read mesh_group " + current_mesh_group->mesh_group_name + " in " + obj_path);
                }
                index += 1;
            }

            // Triangle.
            if (index == 3) {
                vector<glm::dvec3> vertex_from_obj(3);
                vector<glm::dvec3> normal_from_obj(3);
                vector<glm::dvec2> texture_from_obj(3);

                for (auto m = 0; m < 3; m++) {
                    vertex_from_obj[m] = this->obj_vertices[vertex_index[m]];
                    normal_from_obj[m] = this->obj_normals[normal_index[m]];
                    if (texture_index[m] != -1)
                        texture_from_obj[m] = this->obj_texture_indexes[texture_index[m]];
                    else {
                        texture_from_obj[m] = glm::dvec2(MIN_INFINITY_NEGATIVE);
                    }
                }

                Triangle* new_face     = new Triangle(vertex_from_obj, normal_from_obj, texture_from_obj, current_mesh_group);
                new_face->obj_material = current_mtl;

                current_mesh_group->objects.push_back(new_face);
            }
            // Blit into two triangles.
            else if (index == 4) {
                vector<glm::dvec3> vertex_from_obj(3);
                vector<glm::dvec3> normal_from_obj(3);
                vector<glm::dvec2> texture_from_obj(3);

                for (auto m = 0; m < 3; m++) {
                    vertex_from_obj[m] = this->obj_vertices[vertex_index[m]];
                    normal_from_obj[m] = this->obj_normals[normal_index[m]];
                    if (texture_index[m] != -1)
                        texture_from_obj[m] = this->obj_texture_indexes[texture_index[m]];
                    else {
                        texture_from_obj[m] = glm::dvec2(MIN_INFINITY_NEGATIVE);
                    }
                }
                Triangle* new_face = new Triangle(vertex_from_obj, normal_from_obj, texture_from_obj, current_mesh_group);
                current_mesh_group->objects.push_back(new_face);
                new_face->obj_material = current_mtl;

                vertex_index[1]  = vertex_index[2];
                vertex_index[2]  = vertex_index[3];
                normal_index[1]  = normal_index[2];
                normal_index[2]  = normal_index[3];
                texture_index[1] = texture_index[2];
                texture_index[2] = texture_index[3];

                for (auto m = 0; m < 3; m++) {
                    vertex_from_obj[m] = this->obj_vertices[vertex_index[m]];
                    normal_from_obj[m] = this->obj_normals[normal_index[m]];
                    if (texture_index[m] != -1)
                        texture_from_obj[m] = this->obj_texture_indexes[texture_index[m]];
                    else {
                        texture_from_obj[m] = glm::dvec2(MIN_INFINITY_NEGATIVE);
                    }
                }
                new_face               = new Triangle(vertex_from_obj, normal_from_obj, texture_from_obj, current_mesh_group);
                new_face->obj_material = current_mtl;

                current_mesh_group->objects.push_back(new_face);
            }
        }
    }

    if (this->material_mapping.empty()) {
        cout << "Fail to find any material_binding of " << this->obj_path << " and " << this->mtl_path << endl;
    }

    fin.close();
}

void Model::parseMaterial(string mtl_path)
{
    ifstream fin(mtl_path);
    if (!fin.is_open()) {
        throw std::runtime_error("Unable to find mtl file in " + mtl_path);
    }
    string         fileString((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    vector<string> lines = Split(fileString, "\n");

    Material*               current_mtl = NULL;
    string                  current_mtl_name;
    string                  line;
    ParseString             obj_line;
    const unsigned long int line_size  = lines.size();
    unsigned long int       line_index = 0;

    while (line_index < line_size) {
        line = lines[line_index++];
        obj_line.init(line);
        string command = obj_line.GetWord();
        if (command.size() == 0)
            continue;

        // Comment.
        if (command == "#") {
            continue;
        }
        // Mesh group.
        else if (command == "newmtl") {
            if (current_mtl != NULL) {
                this->material_mapping.insert(make_pair(current_mtl_name, *current_mtl));
                delete current_mtl;
            }
            current_mtl_name = obj_line.GetWord();
            current_mtl      = new Material(current_mtl_name);

            continue;
        } else if (command == "Ka") {
            auto x = obj_line.GetFloat();
            auto y = obj_line.GetFloat();
            auto z = obj_line.GetFloat();
            if (0.0 != x || 0.0 != y || 0.0 != z) {
                current_mtl->emission = glm::dvec3(x, y, z);
            }

            continue;
        } else if (command == "Kd") {
            current_mtl->diffuse_reflection[0] = obj_line.GetFloat();
            current_mtl->diffuse_reflection[1] = obj_line.GetFloat();
            current_mtl->diffuse_reflection[2] = obj_line.GetFloat();
            continue;
        } else if (command == "Ks") {
            current_mtl->specular_reflection[0] = obj_line.GetFloat();
            current_mtl->specular_reflection[1] = obj_line.GetFloat();
            current_mtl->specular_reflection[2] = obj_line.GetFloat();

            continue;
        } else if (command == "Ns") {
            current_mtl->shiness = obj_line.GetFloat();
            continue;
        } else if (command == "Ni") {
            current_mtl->refraction_index = obj_line.GetFloat();
            continue;
        } else if (command == "illum") {
            continue;
        } else if (command == "Ke") {
            current_mtl->emission[0] = obj_line.GetFloat();
            current_mtl->emission[1] = obj_line.GetFloat();
            current_mtl->emission[2] = obj_line.GetFloat();

            continue;
        } else if (command == "Tf") {
            current_mtl->transmission_filter[0] = obj_line.GetFloat();
            current_mtl->transmission_filter[1] = obj_line.GetFloat();
            current_mtl->transmission_filter[2] = obj_line.GetFloat();

            continue;
        } else {
            continue;
        }
    }

    // Add last mtl.
    this->material_mapping.insert(make_pair(current_mtl_name, *current_mtl));
    delete current_mtl;

    fin.close();

    generateBSDF();
}

void Model::splitMeshGroup()
{
    size_t g_index = 0;
    for (auto mg : this->all_mesh_groups) {
        for (auto o : mg.objects) {
            o->shapeID = g_index;
        }
        this->all_objs.insert(this->all_objs.end(), mg.objects.begin(), mg.objects.end());

        g_index++;
    }
}

void Model::addSphere(glm::dvec3 pos, double r)
{
    // auto s = new Sphere(pos, r);
    // s->obj_material.diffuse_reflection = glm::dvec3(0.77, 0.79, 0.73);
    ////s->obj_material.emission = glm::dvec3(10, 20, 10);
    // s->obj_material.Fresnel = true;
    // s->obj_material.material_name = "new sphere material";
    // s->obj_material.shiness = 98;
    // s->obj_material.specular_reflection = glm::dvec3(0.97, 0.99, 0.93);
    // s->obj_material.type = MATERIAL_TYPE::GLOSSY;
    // this->all_objs.push_back(s);
}

void Model::generateBSDF()
{
    map<string, Material>::iterator iter = material_mapping.begin();
    for (; iter != material_mapping.end(); iter++) {
        auto& m = iter->second;

        // Glass.
        if (m.refraction_index != 1.0) {
            iter->second.material_type = MType::Glass;
        } else {
            // Phong.
            if (m.specular_reflection != glm::dvec3(0.0)) {
                // Get scale value to ensure energy conservation.
                auto max_value = m.diffuse_reflection + m.specular_reflection;
                auto max_real  = std::fmax(max_value.x, std::fmax(max_value.y, max_value.z));

                iter->second.scale = max_real > 1.0f ? 0.99f * (1.0f / max_real) : 1.0f;

                auto davg                             = getLuminance(m.diffuse_reflection * m.scale);
                auto savg                             = getLuminance(m.specular_reflection * m.scale);
                iter->second.specular_sampling_weight = savg / (savg + davg);

                iter->second.material_type = MType::Phong;
            } else  // Diffuse.
            {
                iter->second.material_type = MType::Diffuse;
            }
        }
    }
}