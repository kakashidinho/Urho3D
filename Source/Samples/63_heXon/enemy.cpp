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

#include "spawnmaster.h"
#include "player.h"
#include "ship.h"
#include "coin.h"
#include "chaomine.h"
#include "mirage.h"

#include "enemy.h"

Enemy::Enemy(Context* context):
    SceneObject(context),
    panicTime_{0.0f},
    initialHealth_{1.0f},
    panic_{0.0f},
    worth_{5},
    lastHitBy_{0},
    whackInterval_{0.5f},
    sinceLastWhack_{0.0f},
    meleeDamage_{0.44f},
    damagePerColorSet_{}
{
}

void Enemy::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

    node_->AddTag("Enemy");
    MC->arena_->AddToAffectors(node_);

    health_ = initialHealth_;


    centerNode_ = node_->CreateChild("SmokeTrail");
    smokeNode_ = centerNode_->CreateChild("Smoke");
    particleEmitter_ = smokeNode_->CreateComponent<ParticleEmitter>();
    particleEffect_ = CACHE->GetTempResource<ParticleEffect>("Particles/Enemy.xml");
    particleEmitter_->SetEffect(particleEffect_);

    //Generate random color
    Randomize();

    if (!sprite_) {

        centerModel_ = centerNode_->CreateComponent<StaticModel>();
        centerModel_->SetModel(MC->GetModel("Core"));
        centerModel_->SetMaterial(MC->GetMaterial("CoreGlow")->Clone());
        centerModel_->GetMaterial(0)->SetShaderParameter("MatDiffColor", color_);
        centerModel_->GetMaterial(0)->SetShaderParameter("MatEmissiveColor", color_);
    }

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetRestitution(0.666f);
    rigidBody_->SetLinearDamping(0.1f);
    rigidBody_->SetMass(2.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    rigidBody_->SetCollisionLayerAndMask(3, M_MAX_UNSIGNED);

    CollisionShape* collider{ node_->CreateComponent<CollisionShape>() };
    collider->SetSphere(2.0f);
    collider->SetPosition(Vector3::UP * 0.23f);


    Node* triggerNode{ node_->CreateChild("TriggerNode") };
    triggerNode->AddTag("Enemy");
    RigidBody* triggerBody{ triggerNode->CreateComponent<RigidBody>() };
    triggerBody->SetKinematic(true);
    triggerBody->SetTrigger(true);
    CollisionShape* triggerShape{ triggerNode->CreateComponent<CollisionShape>() };
    triggerShape->SetSphere(2.333f);

    Node* soundNode{ MC->scene_->CreateChild("SoundSource") };
    soundSource_ = soundNode->CreateComponent<SoundSource>();
    soundSource_->SetGain(0.1f);
    soundSource_->SetSoundType(SOUND_EFFECT);
}
void Enemy::Randomize()
{
    int randomizer{ Random(6) };
    color_ = Color(0.5f + (randomizer * 0.075f),
                   0.9f - (randomizer * 0.075f),
                   0.5f + Max(randomizer - 3.0f, 3.0f) / 6.0f,
                   1.0f);

    Vector<ColorFrame> colorFrames{};
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(color_.r_ * 0.666f,
                                      color_.g_ * 0.666f,
                                      color_.b_ * 0.666f, 0.75f), 0.1f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f));
    particleEffect_->SetColorFrames(colorFrames);
}

void Enemy::Set(const Vector3 position)
{
    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    rigidBody_->ResetForces();
    Randomize();

    lastHitBy_ = 0;
    health_ = initialHealth_;
    panic_ = 0.0f;

    SceneObject::Set(position);
    particleEmitter_->RemoveAllParticles();
    particleEmitter_->SetEmitting(true);
    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Enemy, HandleNodeCollision));

    soundSource_->Stop();
    damagePerColorSet_.Clear();
}

// Takes care of dealing damage and keeps track of who deserves how many points.
void Enemy::Hit(float damage, const int colorSet) {

    lastHitBy_ = colorSet;

    if (damage > health_)
        damage = health_;

    SetHealth(health_ - damage);

    if (colorSet == 0)
        return;

    if (damagePerColorSet_.Contains(colorSet))
        damagePerColorSet_[colorSet] += damage;
    else
        damagePerColorSet_[colorSet] = damage;
}

void Enemy::SetHealth(const float health)
{
    health_ = health;
    panic_ = (initialHealth_ - health_) / initialHealth_;
    if (panic_ < 0.0f)
        panic_ = 0.0f;

    CheckHealth();
}

void Enemy::CheckHealth()
{
    //Die
    if (node_->IsEnabled() && health_ <= 0.0f) {
        Explode();
    }
}

void Enemy::Explode()
{
    int assistColorSet{0};
    for (int colorSet : damagePerColorSet_.Keys())
        if (damagePerColorSet_[colorSet] > initialHealth_ * 0.5f)
            assistColorSet = colorSet;

    int most{ (2 * worth_) / 3 };

    if (assistColorSet != lastHitBy_) {
        if (lastHitBy_ != 0)
            MC->GetPlayerByColorSet(lastHitBy_)->AddScore(most);

        if (assistColorSet != 0)
            MC->GetPlayerByColorSet(assistColorSet)->AddScore(worth_ - most);
    } else {
        if (lastHitBy_ != 0)
            MC->GetPlayerByColorSet(lastHitBy_)->AddScore(worth_);
    }

    GetSubsystem<SpawnMaster>()->Create<Explosion>()
            ->Set(node_->GetPosition(),
                  Color(color_.r_ * color_.r_,
                        color_.g_ * color_.g_,
                        color_.b_ * color_.b_),
                  0.5f * rigidBody_->GetMass(),
                  lastHitBy_, !IsInstanceOf<ChaoMine>());

    if (!IsInstanceOf<ChaoMine>() && !Random(Max((42 - worth_) / 5, 0))) {

        GetSubsystem<SpawnMaster>()->Create<Coin>()->Set(GetPosition());
    }

    Disable();
    RestoreSmoke();
}

void Enemy::RestoreSmoke()
{
    smokeNode_->SetEnabled(true);
    particleEmitter_->SetEmitting(false);
}

Color Enemy::GetGlowColor() const
{
    float factor{ Sin(200.0f * (MC->scene_->GetElapsedTime() + panicTime_)) * (0.25f + panic_ * 0.25f) + (panic_ * 0.5f) };
    factor *= factor * 2.0f;
    return color_ * Max(factor, 0.42f);
}

void Enemy::Update(float timeStep)
{
    float time{MC->scene_->GetElapsedTime() + node_->GetID() * 0.023f};
    panicTime_ += 3.0f * panic_ * timeStep;
    sinceLastWhack_ += timeStep;

    Emerge(timeStep);

    if (!sprite_) {
        //Animate core
        centerModel_->GetMaterial()->SetShaderParameter("VOffset",
                                                        Vector4(0.0f, LucKey::Cycle(time * 3.0f, 0.0f, 1.0f), 0.0f, 0.0f));
        centerNode_->Rotate(Quaternion((1.0f + panic_) * timeStep * 333.0f, Vector3::UP));
    }
}
void Enemy::FixedUpdate(float timeStep)
{ (void)timeStep;

    //Update linear damping
    if (!IsEmerged()) {
        rigidBody_->SetLinearDamping(Min(1.0f, 0.1f - node_->GetPosition().y_ * 0.666f));
    } else {
        rigidBody_->SetLinearDamping(0.1f);
    }

    float emissionRate{ 2.3f + rigidBody_->GetLinearVelocity().Length() * 1.5f };
    particleEffect_->SetMinEmissionRate(emissionRate);
    particleEffect_->SetMaxEmissionRate(emissionRate * 1.23f);
}

void Enemy::HandleNodeCollision(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    Ship* ship{ static_cast<Node*>(eventData[NodeCollision::P_OTHERNODE].GetPtr())->GetComponent<Ship>() };
    if (ship && sinceLastWhack_ > whackInterval_) {

        HitFX* hitFx{ GetSubsystem<SpawnMaster>()->Create<HitFX>() };
        hitFx->Set(eventData[NodeCollision::P_CONTACTS].GetVector3() + GetPosition());
        PlaySample(MC->GetSample("Melee" + String(Random(5) + 1)), 0.16f);
        ship->Hit(meleeDamage_, true);
        sinceLastWhack_ = 0.0f;

    }
}

