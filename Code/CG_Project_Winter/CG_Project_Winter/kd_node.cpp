#include "kd_node.h"

bool KdNode::intersect(Ray& r, Hit& h)
{
    if (!this->bounding_box.intersectAABB(r, h))
        return false;

    bool result = false;
    if (left == NULL && right == NULL) {
        if (!this->objs.empty()) {
            for (auto o : objs)
                result |= o->intersect(r, h);
        }
    } else {
        if (r.getDirection()[this->axis] >= 0) {
            if (left != NULL)
                result |= left->intersect(r, h);
            if (right != NULL)
                result |= right->intersect(r, h);
        } else {
            if (right != NULL)
                result |= right->intersect(r, h);
            if (left != NULL)
                result |= left->intersect(r, h);
        }
    }

    return result;
}

KdNode::KdNode()
{
    this->parent    = NULL;
    this->left      = NULL;
    this->right     = NULL;
    this->axis      = 0;
    this->split_pos = glm::dvec3(0);
    this->level     = 0;
}

KdNode::KdNode(vector<Triangle*>& tris, int axis = 0)
{
    this->parent    = NULL;
    this->left      = NULL;
    this->right     = NULL;
    this->axis      = axis;
    this->split_pos = glm::dvec3(0);
    this->level     = 0;

    this->objs = tris;

    updateBbox();
}

KdNode::~KdNode()
{
}

KdNode* KdNode::getRoot()
{
    KdNode* p = this;
    while (p->parent != NULL) {
        p = p->parent;
    }
    return p;
}

void KdNode::updateTriangleBbox(Triangle* t)
{
    this->bounding_box += t->cal_AABB();
}

void KdNode::mergeBbox(AABB b)
{
    this->bounding_box += b;
}

AABB KdNode::updateBbox()
{
    int num = this->objs.size();
    if (num > 0)
        this->bounding_box = objs[0]->cal_AABB();
    for (auto i = 0; i < num; i++) {
        updateTriangleBbox(objs[i]);
    }
    if (left) {
        mergeBbox(this->left->updateBbox());
    }
    if (right) {
        mergeBbox(this->right->updateBbox());
    }
    return bounding_box;
}

void KdNode::split(int maxdepth)
{
    int num     = this->objs.size();
    this->level = getLevel(this);

    if (num == 0) {
        if (left)
            left->split(maxdepth);

        if (right)
            right->split(maxdepth);
    } else if (num == 1) {
        // this->bounding_box = this->objs[0]->cal_AABB();//only one triangle, narrow the range
        this->left  = NULL;
        this->right = NULL;
    } else if (num >= 2) {
        if (maxdepth >= 0 && level > maxdepth)
            return;

        int ax = level % 3;  // Change from x,y,z.

        std::vector<Triangle*> left_side;
        std::vector<Triangle*> right_side;

        for (auto i = 0; i < num; i++) {
            if (this->objs[i]->cal_AABB()._min[ax] <= this->bounding_box.center()[ax] + EPS) {
                left_side.push_back(objs[i]);
            }
            if (this->objs[i]->cal_AABB()._max[ax] >= this->bounding_box.center()[ax] - EPS) {
                right_side.push_back(objs[i]);
            }
        }

        if (!left_side.empty()) {
            if (NULL == this->left) {
                this->left         = new KdNode(left_side, (ax + 1) % 3);
                this->left->parent = this;
            }
            this->left->bounding_box          = this->bounding_box;
            this->left->bounding_box._max[ax] = this->bounding_box.center()[ax];
            this->left->split_pos             = this->bounding_box.center();

            this->left->split(maxdepth);
        }

        if (!right_side.empty()) {
            if (NULL == this->right) {
                this->right         = new KdNode(right_side, (ax + 1) % 3);
                this->right->parent = this;
            }
            this->right->bounding_box          = this->bounding_box;
            this->right->bounding_box._min[ax] = this->bounding_box.center()[ax];
            this->right->split_pos             = this->bounding_box.center();

            this->right->split(maxdepth);
        }

        // No split possible.
        if (left_side.empty()) {
            this->left = NULL;
        }
        if (right_side.empty()) {
            this->right = NULL;
        }
    }
}

void KdNode::deleteTree(KdNode* root)
{
    if (NULL != root) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;

        if (NULL != root->left)
            root->left = NULL;
        if (NULL != root->right)
            root->right = NULL;
        root = NULL;
    }
}

void KdNode::printTriangleCenters()
{
    printf("\ntriangle centers:\n");
    for (auto o : this->objs) {
        printf("%0.1f %0.1f %0.1f\n", o->cal_AABB().center()[0], o->cal_AABB().center()[1], o->cal_AABB().center()[2]);
    }
}

void KdNode::printTree(KdNode* root)
{
    if (NULL != root) {
        printf("lvl:%d size:%d ", root->getLevel(root), root->objs.size());

        if (root->parent) {
            if (root->parent->left == root)
                printf("node left:");
            else
                printf("node right:");
            printf(" xyz: [%0.1f %0.1f %0.1f] axis: %d parent[%0.1f %0.1f %0.1f] bb[%0.1f %0.1f %0.1f] [%0.1f %0.1f %0.1f]\n",
                   root->bounding_box.center()[0],
                   root->bounding_box.center()[1],
                   root->bounding_box.center()[2],
                   root->axis,
                   root->parent->bounding_box.center()[0],
                   root->parent->bounding_box.center()[1],
                   root->parent->bounding_box.center()[2],
                   root->bounding_box._min[0], root->bounding_box._min[1], root->bounding_box._min[2],
                   root->bounding_box._max[0], root->bounding_box._max[1], root->bounding_box._max[2]);
        } else {
            printf(" xyz: [%0.1f %0.1f %0.1f] axis: %d bb[%0.1f %0.1f %0.1f] [%0.1f %0.1f %0.1f]\n",
                   root->bounding_box.center()[0],
                   root->bounding_box.center()[1],
                   root->bounding_box.center()[2],
                   root->axis,
                   root->bounding_box._min[0], root->bounding_box._min[1], root->bounding_box._min[2],
                   root->bounding_box._max[0], root->bounding_box._max[1], root->bounding_box._max[2]);
        }

        printTree(root->left);
        printTree(root->right);
    }
}

int KdNode::getDepth(KdNode* n, int depth = 0)
{
    if (NULL == n)
        return depth;

    int depth1 = depth;
    int depth2 = depth;

    if (n->left) {
        depth1 += getDepth(n->left, depth++);
    }
    if (n->right) {
        depth2 += getDepth(n->right, depth2++);
    }

    return (depth1 > depth2 ? depth1 : depth2);
}

int KdNode::getLevel(KdNode* n)
{
    if (n == NULL)
        return 0;

    int level = 0;

    KdNode* node = n;

    while (node->parent != NULL) {
        level++;
        node = node->parent;
    }

    return level;
}

void KdNode::add(Triangle* t)
{
    this->objs.push_back(t);
}