#pragma once
#include <vector>

#include "object3d.h"
#include "triangle.h"

class KdNode
{
public:
    bool intersect(Ray& r, Hit& h);

    KdNode();
    KdNode(vector<Triangle*>& tris, int axis);
    ~KdNode();

    KdNode* getRoot();
    void    updateTriangleBbox(Triangle* t);
    void    mergeBbox(AABB b);
    AABB    updateBbox();
    void    split(int maxdepth);
    void    deleteTree(KdNode* root);
    void    printTriangleCenters();
    void    printTree(KdNode* root);
    int     getDepth(KdNode* n, int depth);
    int     getLevel(KdNode* n);
    void    add(Triangle* t);

public:
    vector<Triangle*> objs;
    AABB              bounding_box;

    KdNode* left   = NULL;
    KdNode* right  = NULL;
    KdNode* parent = NULL;

    int        axis;
    glm::dvec3 split_pos;

    int level;
};