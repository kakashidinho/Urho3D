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

#include "splatterpillar.h"

#include "pilot.h"

void SplatterPillar::RegisterObject(Context *context)
{
    context->RegisterFactory<SplatterPillar>();

    for (int i{1}; i < 6; ++i)
        MC->GetSample("Splatter" + String(i));
}

SplatterPillar::SplatterPillar(Context* context):
    LogicComponent(context),
    playerId_{0},
    spun_{false},
    reset_{true},
    delay_{0.5f},
    delayed_{0.0f},
    sequenceLength_{5.0f},
    lastTriggered_{-sequenceLength_},
    rotationSpeed_{}
{

}

void SplatterPillar::OnNodeSet(Node *node)
{ if (!node) return;

    playerId_ = node_->GetPosition().x_ < 0.0f ? 1 : 2;

    node_->Rotate(Quaternion(Random(6) * 60.0f, Vector3::UP));
    pillarNode_ = node_->CreateChild("Pillar");
    bloodNode_ = node_->CreateChild("Blood");
    bloodNode_->SetScale(1.23f);
    pillar_ = pillarNode_->CreateComponent<AnimatedModel>();
    pillar_->SetModel(MC->GetModel("SplatterPillar"));
    pillar_->SetMorphWeight(0, 0.0f);
    pillar_->SetCastShadows(true);

    pillar_->SetMaterial(0, MC->GetMaterial("Basic"));
    pillar_->SetMaterial(1, MC->GetMaterial("BlueGlow"));

    pillar_->SetMaterial(2, MC->GetMaterial("Metal"));
    pillar_->SetMaterial(3, MC->GetMaterial("Drain"));

    blood_ = bloodNode_->CreateComponent<AnimatedModel>();
    blood_->SetEnabled(false);
    blood_->SetCastShadows(true);
    blood_->SetModel(MC->GetModel("Blood"));
    blood_->SetMaterial(0, MC->GetMaterial("Blood")->Clone());

    particleNode_ = node_->CreateChild("BloodParticles");
    particleNode_->Translate(Vector3::UP * 2.3f);
    splatEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    splatEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/BloodSplat.xml"));
    splatEmitter_->SetEmitting(false);
    dripEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    dripEmitter_->SetEffect(CACHE->GetTempResource<ParticleEffect>("Particles/BloodDrip.xml"));
    dripEmitter_->SetEmitting(false);

    soundSource_ = node_->CreateComponent<SoundSource>();

    RigidBody* triggerBody{ node_->CreateComponent<RigidBody>() };
    triggerBody->SetTrigger(true);
    CollisionShape* trigger{ node_->CreateComponent<CollisionShape>() };
    trigger->SetSphere(0.13f, Vector3::UP * 0.42f);
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(SplatterPillar, Trigger));

}

void SplatterPillar::Trigger(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    Node* otherNode{ static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr()) };
    if (otherNode->HasComponent<Pilot>()) {

        otherNode->GetComponent<Pilot>()->Upload();

        rotationSpeed_ = Random(-1.0f, 1.0f);
        lastTriggered_ = MC->scene_->GetElapsedTime();
        bloodNode_->Rotate(Quaternion(Random(360.0f), Vector3::UP));
        blood_->SetEnabled(true);
        splatEmitter_->SetEmitting(true);
        soundSource_->Play(MC->GetSample("Splatter" + String(Random(1, 6))));
    }
}

void SplatterPillar::Update(float timeStep)
{ (void)timeStep;

    if (MC->GetGameState() != GS_LOBBY) return;

    float elapsedTime{MC->scene_->GetElapsedTime()};
    float intoSequence{(elapsedTime - lastTriggered_)/sequenceLength_};
    unsigned numMorphs{blood_->GetNumMorphs()};

    //Animate
    if (intoSequence < 1.0f) {
        //Animate blood
        if (!bloodNode_->IsEnabled()){
            bloodNode_->SetEnabled(true);
            particleNode_->Rotate(Quaternion{Random(360.0f), Vector3::UP});
        }
        if (intoSequence > 0.023f) {
            splatEmitter_->SetEmitting(false);
            dripEmitter_->SetEmitting(true);
        }
        for (unsigned m{0}; m < numMorphs; ++m){
            float intoMorph{Clamp(intoSequence * numMorphs - m, 0.0f, 2.0f)};
            if (intoMorph > 1.0f) intoMorph = Max(2.0f - intoMorph, 0.0f);
            else if (m == 0) Min(intoMorph *= 5.0f, 1.0f);
            blood_->SetMorphWeight(m, Clamp(intoMorph, 0.0f, 1.0f));
        }
        blood_->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.23f, 0.32f, 0.32f, Clamp(1.0f - (intoSequence - 0.75f) * 5.0f, 0.0f, 1.0f)));
        blood_->GetMaterial()->SetShaderParameter("Dissolve", 0.75f * intoSequence + 0.23f);
        ParticleEffect* dripEffect{dripEmitter_->GetEffect()};
        dripEffect->SetEmitterSize(Vector3{1.0f - intoSequence, 0.0f, 1.0f - intoSequence});
        dripEffect->SetMinEmissionRate(Max(123.0f - 200.0f * intoSequence, 0.0f));
        dripEffect->SetMaxEmissionRate(Max(320.0f - 340.0f * intoSequence, 0.0f));
        //Animate pillar
        if      (intoSequence < 0.125f)
            pillar_->SetMorphWeight(0, Clamp(123.0f * intoSequence, 0.0f, 1.0f));
        else if (intoSequence < 0.1666f) {
            pillar_->SetMorphWeight(0, 1.0f);
            if (!spun_){
                pillarNode_->Rotate(Quaternion{Random(6)*60.0f, Vector3::UP});
                spun_ = true;
            }
        }
        else if (intoSequence > (1.0f / 6.0f)) {
            spun_ = false;
            float weight{Max(2.0f * (1.0f - 3.0f * intoSequence), 0.0f)};
            pillar_->SetMorphWeight(0, Clamp(weight * weight * weight, 0.0f, 1.0f));
        }
    } else {
    //When idle
        //Reset
        if (bloodNode_->IsEnabled()) {
            bloodNode_->SetEnabled(false);
            dripEmitter_->SetEmitting(false);
        }
        if (pillar_->GetMorphWeight(0) != 0.0f) pillar_->SetMorphWeight(0, 0.0f);
    }
}

bool SplatterPillar::IsIdle() const
{
    return !bloodNode_->IsEnabled();
}
