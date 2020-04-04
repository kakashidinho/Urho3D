#include "fish.h"

Fish::Fish(Context* context) :
    LogicComponent(context),
    verticalSpeed_{0.0f},
    jumpDelay_{0.0f}
{
    SetUpdateEventMask(USE_UPDATE);
}

void Fish::RegisterObject(Context* context)
{
    context->RegisterFactory<Fish>();
}

void Fish::OnNodeSet(Node* node)
{ (void)node;

    CreateFish();

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Fish, HandleCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISIONEND, URHO3D_HANDLER(Fish, HandleCollisionEnd));
}

void Fish::CreateFish()
{
    AnimatedModel* urhoObject{node_->CreateComponent<AnimatedModel>()};
    urhoObject->SetModel(CACHE->GetResource<Model>("Models/Urho.mdl"));
    urhoObject->SetCastShadows(true);
    node_->SetRotation(URHO_DEFAULT_ROTATION);

    urhoObject->ApplyMaterialList();

    AnimationController* animCtrl{node_->CreateComponent<AnimationController>()};
    animCtrl->PlayExclusive("Models/Swim.ani", 0, true);
    animCtrl->SetSpeed("Models/Swim.ani", 1.23f);

    RigidBody* body{node_->CreateComponent<RigidBody>()};
    body->SetMass(1.0f);
    body->SetKinematic(true);

    CollisionShape* shape1{node_->CreateComponent<CollisionShape>()};
    shape1->SetShapeType(SHAPE_CAPSULE);
    shape1->SetSize(Vector3(2.0f, 3.8f, 0.0f));
    shape1->SetPosition(Vector3(0.0f, 0.1f, -0.2f));
    shape1->SetRotation(Quaternion(90.f, 0.0f, 0.0f));
}

void Fish::HandleCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    Node* otherNode{static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr())};

    if (otherNode->GetName() == "Net")
        GLOBAL->neededGameState_ = GS_DEAD;
}

void Fish::HandleCollisionEnd(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    Node* otherNode{static_cast<Node*>(eventData[NodeCollisionEnd::P_OTHERNODE].GetPtr())};

    if (otherNode->GetName() == "Barrier")
    {
        if (GLOBAL->gameState_ == GS_PLAY){
            GLOBAL->SetScore(GLOBAL->GetScore() + 1);
            SoundSource* soundSource{ otherNode->GetOrCreateComponent<SoundSource>() };
            if (GLOBAL->GetScore() == GLOBAL->GetHighscore() + 1) {
                soundSource->Play(CACHE->GetResource<Sound>("Samples/King.ogg"));
            } else {
                soundSource->Play(CACHE->GetResource<Sound>("Samples/Pass.ogg"));
            }
        }
    }
}

void Fish::Reset()
{
    node_->SetPosition(Vector3::ZERO);
    node_->SetRotation(URHO_DEFAULT_ROTATION);

    verticalSpeed_ = 0.0f;
    jumpDelay_ = 0.0f;
}

void Fish::Update(float timeStep)
{
    if (GLOBAL->gameState_ == GS_DEAD)
    {
        node_->Rotate(Quaternion(0.0f, timeStep * 200.0f, -timeStep * 400.0f));

        if (node_->GetPosition().z_ > -50.0f)
        {
            node_->Translate(Vector3(0.0f, -10.0f * timeStep, -30.0f * timeStep), TS_WORLD);
        }

        return;
    }

    AnimationController* animationController{ node_->GetComponent<AnimationController>() };
    animationController->SetSpeed("Models/Swim.ani", Clamp(0.1f * (verticalSpeed_ + 23.0f), 0.0f, 5.0f));

    if (GLOBAL->gameState_ != GS_PLAY)
        return;

    Vector3 pos{ node_->GetPosition() };

    verticalSpeed_ -= timeStep * GRAV_ACC;
    
    if (jumpDelay_ > 0.0f)
        jumpDelay_ -= timeStep;

    if ((INPUT->GetMouseButtonPress(MOUSEB_LEFT) || ( INPUT->GetNumTouches() > 0)) && jumpDelay_ <= 0.0f)
    {
        verticalSpeed_ = UP_SPEED;
        SoundSource* soundSource{node_->GetOrCreateComponent<SoundSource>()};
        soundSource->Play(CACHE->GetResource<Sound>("Samples/Blup" + String{Random(4)} + ".ogg"));

        jumpDelay_ = 0.75f;
    }

    pos += Vector3::UP * verticalSpeed_ * timeStep;
    node_->SetPosition(pos);
    float xRot{Clamp(Lerp(0.0f, 34.0f * verticalSpeed_, Clamp(timeStep * 2.0f, 0.0f, 0.666f)), -13.0f, 13.0f)};
    node_->SetRotation(Quaternion(xRot, -90.0f, 0.0f));

    AnimatedModel* animatedModel{node_->GetComponent<AnimatedModel>()};
    if (!animatedModel->IsInView())
        GLOBAL->neededGameState_ = GS_DEAD;
}
