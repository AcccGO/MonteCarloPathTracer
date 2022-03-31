#include "hit.h"
Hit::Hit()
{
    this->intersection_object       = NULL;
    this->t                         = 0.0;
    this->intersection_point_normal = glm::dvec3(0.0);
    this->intersection_point        = glm::dvec3(0);
}

Hit::Hit(double _t, Triangle *o)
{
    this->t                   = _t;
    this->intersection_object = o;

    // Default is 0 !!!
    this->intersection_point_normal = glm::dvec3(0.0);
    this->intersection_point        = glm::dvec3(0);
}

Hit::Hit(const Hit &h)
{
    this->t                         = h.t;
    this->intersection_object       = h.intersection_object;
    this->intersection_point        = h.intersection_point;
    this->intersection_point_normal = h.intersection_point_normal;

    this->sampled_component = h.sampled_component;
    this->sampled_type      = h.sampled_type;
    this->wo                = h.wo;
    this->wi                = h.wi;
    this->u                 = h.u;
    this->v                 = h.v;
    this->frame_ng          = h.frame_ng;
    this->frame_ns          = h.frame_ns;
}

Hit::~Hit()
{
    this->intersection_object = NULL;
}