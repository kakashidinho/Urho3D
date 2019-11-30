#ifndef ENVIRONMENTLOGIC_H
#define ENVIRONMENTLOGIC_H

#include "global.h"

class Environment : public LogicComponent
{
    URHO3D_OBJECT(Environment, LogicComponent);

public:
    Environment(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
};

#endif // ENVIRONMENTLOGIC_H
