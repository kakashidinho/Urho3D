/* heXon
// Copyright (C) 2018 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "hitfx.h"
#include "arena.h"
#include "player.h"
#include "ship.h"
#include "chaomine.h"
#include "soundeffect.h"
#include "spawnmaster.h"

#include "seeker.h"

void Seeker::RegisterObject(Context *context)
{
    context->RegisterFactory<Seeker>();

    MC->GetSample("Seeker");
}

Seeker::Seeker(Context* context):
    SceneObject(context),
    tailGens_{},
    age_{0.0f},
    lifeTime_{4.2f},
    damage_{1.23f}
{

}

void Seeker::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

    node_->SetName("Seeker");
    MC->arena_->AddToAffectors(node_);

    big_ = false;

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(1.23f);
    rigidBody_->SetLinearDamping(0.42f);
    rigidBody_->SetTrigger(true);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);

    CollisionShape* trigger{ node_->CreateComponent<CollisionShape>() };
    trigger->SetSphere(0.42f);

    ParticleEmitter* particleEmitter{ node_->CreateComponent<ParticleEmitter>() };
    particleEmitter->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/Seeker.xml"));

    AddTails();

    Light* light{ node_->CreateComponent<Light>() };
    light->SetRange(5.0f);
    light->SetBrightness(1.3f);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
}

void Seeker::Update(float timeStep)
{
    age_ += timeStep;
    if (age_ > lifeTime_ && node_->IsEnabled()) {
        HitFX* hitFx{ GetSubsystem<SpawnMaster>()->Create<HitFX>() };
        hitFx->Set(GetPosition(), 0, false);
        Disable();
    }

    node_->LookAt(node_->GetWorldPosition() + rigidBody_->GetLinearVelocity().Normalized());

    for (TailGenerator* tg : {tailGens_.first_, tailGens_.second_} ) {
        if (tg) {
            float speed{ rigidBody_->GetLinearVelocity().Length() };
            tg->SetTailLength(speed * 0.027f + 0.055f);
            tg->SetWidthScale(MC->Sine(speed * 2.3f, 0.42f, 0.666f / (1.0f + speed), Random(0.05f)));
        }
    }
}

void Seeker::FixedUpdate(float timeStep)
{
    rigidBody_->ApplyForce((TargetPosition() - node_->GetPosition()).Normalized() * 6.0f);
}

void Seeker::HandleTriggerStart(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    if (!node_->IsEnabled())
        return;

    PODVector<RigidBody*> collidingBodies{};
    rigidBody_->GetCollidingBodies(collidingBodies);

    for (unsigned i{0}; i < collidingBodies.Size(); ++i) {
        RigidBody* collider{ collidingBodies[i] };
        if (collider->GetNode()->HasComponent<Ship>()) {
            Ship* hitShip{ collider->GetNode()->GetComponent<Ship>() };

            hitShip->Hit(damage_, false);

            GetSubsystem<SpawnMaster>()->Create<HitFX>()
                    ->Set(node_->GetPosition(), 0, false);
            collider->ApplyImpulse(rigidBody_->GetLinearVelocity() * 0.5f);
            Disable();
        }
        else if (collider->GetNode()->HasComponent<ChaoMine>()){
            collider->GetNode()->GetComponent<ChaoMine>()->Hit(damage_, 0);
        }
        else if (collider->GetNode()->HasComponent<Seeker>()) {

            GetSubsystem<SpawnMaster>()->Create<HitFX>()
                    ->Set(node_->GetPosition(), 0, false);

            Disable();
        }
    }
}
Vector3 Seeker::TargetPosition()
{
    Player* nearestPlayer{ MC->GetNearestPlayer(GetPosition()) };
    if (nearestPlayer)
        return nearestPlayer->GetPosition();
    else
        return Vector3::ZERO;
}

void Seeker::Set(Vector3 position, bool sound)
{
    age_ = 0.0f;
    SceneObject::Set(position);
    rigidBody_->ResetForces();
    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    AddTails();

    if (sound) {
//        PlaySample(MC->GetSample("Seeker"), 0.666f);
        SoundEffect* seekerSound{ SPAWN->Create<SoundEffect>() };
        seekerSound->Set(node_->GetWorldPosition());
        seekerSound->PlaySample(MC->GetSample("Seeker"), 0.666f);
    }

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Seeker, HandleTriggerStart));

    rigidBody_->ApplyImpulse((TargetPosition() - node_->GetPosition()).Normalized() * 2.3f);
}
void Seeker::Disable()
{
    RemoveTails();
    SceneObject::Disable();
}

void Seeker::SetLinearVelocity(const Vector3& velocity)
{
    rigidBody_->SetLinearVelocity(velocity);
}

void Seeker::AddTails()
{
    RemoveTails();

    for (bool first : {true, false}) {

        TailGenerator* tg{ node_->CreateComponent<TailGenerator>() };
        tg->SetMatchNodeOrientation(true);
        tg->SetDrawHorizontal(true);
        tg->SetDrawMirrored(first);
        tg->SetDrawVertical(false);
        tg->SetNumTails(5);
        tg->SetColorForHead(Color(0.666f, 0.42f, 1.0f, 1.0f));
        tg->SetColorForTip(Color(0.0f, 0.1f, 0.23f, 0.42f));

        if (first)
            tailGens_.first_ = tg;
        else
            tailGens_.second_ = tg;
    }
}
void Seeker::RemoveTails()
{
    if (tailGens_.first_) {
        tailGens_.first_->Remove();
        tailGens_.first_ = nullptr;
    }
    if (tailGens_.second_) {
        tailGens_.second_->Remove();
        tailGens_.second_ = nullptr;
    }
}
