#ifndef CROWN_H
#define CROWN_H

#include "global.h"


class Crown : public LogicComponent
{
    URHO3D_OBJECT(Crown, LogicComponent);
public:
    Crown(Context* context);
    static void RegisterObject(Context* context);
    virtual void Update(float timeStep);
    virtual void OnNodeSet(Node* node);
    void Reset();
};

#endif // CROWN_H
