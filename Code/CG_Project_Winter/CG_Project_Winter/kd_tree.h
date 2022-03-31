#pragma once
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "kd_node.h"

using namespace std;

class KdTree
{
public:
    bool intersect(Ray& r, Hit& h);

    KdTree();
    KdTree(vector<Triangle*>& objs);
    ~KdTree();

    void deleteTree(KdNode* root);
    void printToFile(KdNode* root, ofstream& fileout);
    void writeKDtoFile(KdNode* root, const char* path);
    void printTree();
    void split(int maxdepth);

public:
    KdNode* root_node;
};