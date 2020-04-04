#include "ship.h"
#include "hitfx.h"
#include "spawnmaster.h"

#include "coin.h"

StaticModelGroup* Coin::coinGroup_{};

void Coin::RegisterObject(Context* context)
{
    context->RegisterFactory<Coin>();

    MC->GetSample("Coin");
}

Coin::Coin(Context* context) : SceneObject(context),
    bubbleEmitter_{}
{
    big_ = false;
}

void Coin::OnNodeSet(Node* node)
{ if (!node) return;

    bubbleEmitter_ = node_->CreateChild("Bubbles")->CreateComponent<ParticleEmitter>();
    bubbleEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/Bubbles.xml"));

    if (!coinGroup_) {
        coinGroup_ = MC->scene_->CreateComponent<StaticModelGroup>();
        coinGroup_->SetModel(MC->GetModel("Coin"));
        coinGroup_->SetMaterial(MC->GetMaterial("ChromeEnvmap"));
    }

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(0.42f);
    rigidBody_->SetLinearDamping(0.88f);
    rigidBody_->SetAngularDamping(0.13f);
    rigidBody_->SetTrigger(true);

    CollisionShape* collisionShape{ node_->CreateComponent<CollisionShape>() };
    collisionShape->SetSphere(0.75f);
}

void Coin::Set(const Vector3 position)
{
//    node_->GetComponent<ParticleEmitter>()->RemoveAllParticles();
    bubbleEmitter_->SetEmitting(true);

    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    rigidBody_->SetAngularVelocity(Vector3::ZERO);
    rigidBody_->ResetForces();
    SceneObject::Set(position);
    node_->SetRotation(Quaternion::IDENTITY);
    rigidBody_->ApplyTorqueImpulse(Vector3(LucKey::RandomSign() * Random(0.034f, 0.05f),
                                          Random(-0.005f, 0.005f),
                                          LucKey::RandomSign() * Random(0.023f, 0.06f)));

    node_->Translate(Vector3(Random(-0.42f, 0.42f), 1.0f, Random(-0.42f, 0.42f)));
    rigidBody_->ApplyImpulse((GetPosition() - position).Normalized());

    coinGroup_->AddInstanceNode(node_);
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Coin, HandleNodeCollisionStart));
}

void Coin::Disable()
{
    coinGroup_->RemoveInstanceNode(node_);
    SceneObject::Disable();

    bubbleEmitter_->GetNode()->SetEnabled(true);
    bubbleEmitter_->SetEmitting(false);
}


void Coin::Update(float)
{
    if (GetPosition().y_ < -23.0f) {
        Disable();
    }

    ParticleEffect* bubbles{ bubbleEmitter_->GetEffect() };
    bubbles->SetMinEmissionRate(Max(0.0f, Min(GetPosition().y_ + 2.3f, 2.3f * (rigidBody_->GetLinearVelocity().Length() - 3.0f))));
    bubbles->SetMaxEmissionRate(Max(0.0f, Min(GetPosition().y_ + 4.2f, 2.3f * (rigidBody_->GetLinearVelocity().Length() - 2.0f))));}

void Coin::HandleNodeCollisionStart(StringHash, VariantMap& eventData)
{
    Node* otherNode{ static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr()) };

    if (otherNode->HasComponent<Ship>()) {

        Ship* ship{ otherNode->GetComponent<Ship>() };
        ship->GetPlayer()->AddScore(10);
        ship->PlaySample(MC->GetSample("Coin"), 0.12f);
        GetSubsystem<SpawnMaster>()->Create<HitFX>()->Set(GetPosition(), ship->GetColorSet());
        Disable();
    }
}

void Coin::Launch()
{
    rigidBody_->ApplyImpulse(Vector3::UP * 17.f);
}
