#ifndef CREDIT_H
#define CREDIT_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

class Coin : public SceneObject
{
    URHO3D_OBJECT(Coin, SceneObject);
public:
    Coin(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float) override;
    void Set(const Vector3 position) override;
    void Disable() override;
    void Launch();

    void HandleNodeCollisionStart(StringHash, VariantMap& eventData);
private:
    static StaticModelGroup* coinGroup_;

    RigidBody* rigidBody_;
    ParticleEmitter* bubbleEmitter_;
};

#endif // CREDIT_H
