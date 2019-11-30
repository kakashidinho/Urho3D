#ifndef FISHLOGIC_H
#define FISHLOGIC_H

#include "global.h"

class Fish : public LogicComponent
{
    URHO3D_OBJECT(Fish, LogicComponent);

public:
    Fish(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node);
    void Update(float timeStep);
    void Reset();
    void HandleCollisionStart(StringHash eventType, VariantMap& eventData);
    void HandleCollisionEnd(StringHash eventType, VariantMap& eventData);

private:
    float verticalSpeed_;

    float jumpDelay_;
    void CreateFish();
};

#endif // FISHLOGIC_H
