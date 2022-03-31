#include "kd_tree.h"

bool KdTree::intersect(Ray& r, Hit& h)
{
    return this->root_node->intersect(r, h);
}

KdTree::KdTree()
{
    this->root_node = new KdNode();
}

KdTree::KdTree(vector<Triangle*>& objs)
{
    this->root_node       = new KdNode();
    this->root_node->objs = objs;
}

KdTree::~KdTree()
{
    this->deleteTree(this->root_node);
}

void KdTree::deleteTree(KdNode* root)
{
    if (root != NULL) {
        deleteTree(root->left);
        deleteTree(root->right);
        // delete root;

        if (root->left != NULL)
            root->left = NULL;
        if (root->right != NULL)
            root->right = NULL;

        delete root;
        root = NULL;
    }
}

void KdTree::printToFile(KdNode* root, ofstream& fileout)
{
    if (root != NULL) {
        fileout << root->bounding_box._min[0] << " " << root->bounding_box._min[1] << " " << root->bounding_box._min[2] << " "
                << root->bounding_box._max[0] << " " << root->bounding_box._max[1] << " " << root->bounding_box._max[2] << "\n";

        printToFile(root->left, fileout);
        printToFile(root->right, fileout);
    }
}

void KdTree::writeKDtoFile(KdNode* root, const char* path)
{
    ofstream fileout;
    fileout.open(path);

    printToFile(root, fileout);

    fileout.close();
}

void KdTree::printTree()
{
    if (this->root_node) {
        this->root_node->printTree(this->root_node->getRoot());
    }
}

void KdTree::split(int maxdepth)
{
    this->root_node->split(maxdepth);
}