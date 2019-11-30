#include "environment.h"

Environment::Environment(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Environment::RegisterObject(Context* context)
{
    context->RegisterFactory<Environment>();
}

void Environment::Update(float timeStep)
{
    if (GLOBAL->gameState_ == GS_PLAY || GLOBAL->gameState_ == GS_INTRO)
        node_->Rotate(Quaternion(0.0f, -timeStep * BAR_SPEED * 0.42f, 0.0f));
}
