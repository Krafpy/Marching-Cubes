#include "ControlNode.h"

#include <vec3d.h>

ControlNode::ControlNode(vec3d& _pos, float _value, bool _active) : pos(_pos), value(_value), active(_active)
{

}

ControlNode::ControlNode()
{

}

ControlNode::~ControlNode()
{

}
