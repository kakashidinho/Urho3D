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

#include "arena.h"
#include "TailGenerator.h"
#include "hitfx.h"
#include "chaomine.h"
#include "spire.h"
#include "seeker.h"
#include "phaser.h"
#include "mason.h"
#include "ship.h"
#include "spawnmaster.h"

#include "brick.h"

void Brick::RegisterObject(Context* context)
{
    context->RegisterFactory<Brick>();
}

Brick::Brick(Context* context) : SceneObject(context),
    damage_{3.4f},
    spikeMaterial_{},
    particleEmitter_{},
    trigger_{},
    traveled_{}
{

}

void Brick::OnNodeSet(Node* node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

    MC->arena_->AddToAffectors(node_);

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(2.3f);
    rigidBody_->SetLinearDamping(0.23f);
    rigidBody_->SetTrigger(true);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);

    trigger_ = node_->CreateComponent<CollisionShape>();
    trigger_->SetBox(Vector3(0.666f, 3.4f, 0.23f));

    Node* spikeNode{ node_->CreateChild("Spike")};
    StaticModel* spikeModel{ spikeNode->CreateComponent<StaticModel>() };
    spikeModel->SetModel(MC->GetModel("Spike"));
    spikeMaterial_ = MC->GetMaterial("Spike")->Clone();
    spikeModel->SetMaterial(spikeMaterial_);

    Node* particleNode{ node_->CreateChild("Particles") };
    particleNode->SetPosition(Vector3::UP * 0.42f);
    particleEmitter_ = particleNode->CreateComponent<ParticleEmitter>();
    particleEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/Brick.xml")->Clone());

    Light* light{ node_->CreateComponent<Light>() };
    light->SetRange(2.3f);
    light->SetBrightness(3.4f);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
}

void Brick::Set(Vector3 position, Vector3 direction)
{
    SceneObject::Set(position);

    traveled_ = 0.0f;

    rigidBody_->ResetForces();
    rigidBody_->SetLinearVelocity(Vector3::ZERO);

    particleEmitter_->RemoveAllParticles();
    particleEmitter_->SetEmitting(true);

    node_->LookAt(position + direction);
    particleEmitter_->GetEffect()->SetMinDirection(direction + Vector3::UP);
    particleEmitter_->GetEffect()->SetMaxDirection(direction + Vector3::UP);

    rigidBody_->ApplyImpulse(direction * 123.0f);

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Brick, HandleTriggerStart));
}
void Brick::FixedPostUpdate(float timeStep)
{
    float growth{ Clamp(traveled_, 0.1f, 1.0f) };
    trigger_->SetBox(Vector3(0.666f, 3.4f, 4.2f * growth), Vector3::BACK * 2.3f * growth);
}
void Brick::HandleTriggerStart(StringHash, VariantMap&)
{

    if (!node_->IsEnabled())
        return;

    PODVector<RigidBody*> collidingBodies{};
    rigidBody_->GetCollidingBodies(collidingBodies);

    for (unsigned i{0}; i < collidingBodies.Size(); ++i) {

        RigidBody* collider{ collidingBodies[i] };
        Node* collidingNode{ collider->GetNode() };

        if (collidingNode->HasComponent<Ship>()) {

            collidingNode->GetComponent<Ship>()->Hit(damage_, false);
            collider->ApplyImpulse(rigidBody_->GetLinearVelocity() * 0.5f);

            Disable();

        } else if (collidingNode->HasComponent<ChaoMine>()) {

            collidingNode->GetComponent<ChaoMine>()->Hit(damage_, 0);

        } else if (Spire* spire = collider->GetNode()->GetComponent<Spire>()) {

            if (node_->GetDirection().ProjectOntoAxis((spire->GetPosition() - node_->GetPosition()).Normalized()) > 0.0f) {

                spire->Shoot(false)->SetLinearVelocity(rigidBody_->GetLinearVelocity() * 0.23f);
                Disable();
            }
        }
    }
}

void Brick::Disable()
{
//    GetSubsystem<SpawnMaster>()->Create<HitFX>()
//            ->Set(node_->GetPosition(), 0, false);
    Phaser* phaser{ GetSubsystem<SpawnMaster>()->Create<Phaser>() };
    phaser->Set(MC->GetModel("Spike"), GetPosition(), rigidBody_->GetLinearVelocity() * 0.23f, false, false);

    SceneObject::Disable();

    particleEmitter_->GetNode()->SetEnabled(true);
    particleEmitter_->SetEmitting(false);
}

void Brick::Update(float timeStep)
{
    spikeMaterial_->SetShaderParameter("MatEmissiveColor", Color(Random(0.23f, 1.0f), 0.666f, Random(0.666f, 1.0f)));
    traveled_ += rigidBody_->GetLinearVelocity().Length() * timeStep;

//    if (traveled_ > 35.0f)
//        Disable();
}
