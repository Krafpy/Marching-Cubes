#ifndef CONTROLNODE_H
#define CONTROLNODE_H

#include <vec3d.h>

// control nodes (i.e. cube vertex) with its state (active or not)

class ControlNode
{
    public:
        vec3d pos;
        float value;
        bool active;

        ControlNode();
        ControlNode(vec3d& _pos, float _value, bool _active);
        virtual ~ControlNode();

    protected:

    private:
};

#endif // CONTROLNODE_H
