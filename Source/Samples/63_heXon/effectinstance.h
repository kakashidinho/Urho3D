#ifndef EFFECTINSTANCE_H
#define EFFECTINSTANCE_H

#include <Urho3D/Urho3D.h>

#include "effect.h"

class EffectInstance : public Effect
{
    URHO3D_OBJECT(EffectInstance, Effect);
public:
    static void RegisterObject(Context* context);
    EffectInstance(Context* context);
    virtual void Set(const Vector3 position, ParticleEffect* effect);
};

#endif // EFFECTINSTANCE_H
