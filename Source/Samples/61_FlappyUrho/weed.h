#ifndef WEED_H
#define WEED_H

#include "global.h"


class Weed : public LogicComponent
{
    URHO3D_OBJECT(Weed, LogicComponent);
public:
    Weed(Context* context);
    static void RegisterObject(Context* context);
    virtual void Update(float timeStep);
    virtual void OnNodeSet(Node* node);
};

#endif // WEED_H
