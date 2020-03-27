#include "effectinstance.h"

void EffectInstance::RegisterObject(Context* context)
{
    context->RegisterFactory<EffectInstance>();
}

EffectInstance::EffectInstance(Context* context) : Effect(context)
{

}

void EffectInstance::Set(const Vector3 position, ParticleEffect* effect)
{
    GetEmitter()->SetEffect(effect);

    Effect::Set(position);
}
