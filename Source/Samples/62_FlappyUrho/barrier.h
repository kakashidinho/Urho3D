#ifndef BARRIERLOGIC_H
#define BARRIERLOGIC_H

#include "global.h"

class Barrier : public LogicComponent
{
    URHO3D_OBJECT(Barrier, LogicComponent);

public:
    Barrier(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
    void OnNodeSet(Node* node);
private:
    static StaticModelGroup* netGroup_;

};

#endif // BARRIERLOGIC_H
