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

#include "inputmaster.h"
#include "player.h"

#include "phaser.h"

void Phaser::RegisterObject(Context *context)
{
    context->RegisterFactory<Phaser>();

    MC->GetSample("Flash");
}

Phaser::Phaser(Context* context) : Effect(context),
    staticModel_{},
    phaseMaterial_{ MC->GetMaterial("Phase")->Clone() },
    velocity_{},
    spin_{},
    stateChanger_{}
{
}

void Phaser::OnNodeSet(Node *node)
{ if (!node) return;

    Effect::OnNodeSet(node);

    node_->SetName("Phaser");
    staticModel_ = node_->CreateComponent<StaticModel>();
}

void Phaser::Set(Model* model, const Vector3 position, const Vector3 velocity, const bool stateChanger, bool audible)
{
    stateChanger_ = stateChanger;

    Effect::Set(position);

    velocity_ = velocity;
    spin_ = Quaternion::IDENTITY;

    node_->LookAt(position + velocity);

    staticModel_->SetModel(model);
    staticModel_->SetMaterial(phaseMaterial_);

    if (audible) {
        PlaySample(MC->GetSample("Flash"), 0.23f);
    }
}
void Phaser::Set(Model* model, const Vector3 position, const Quaternion rotation, const Vector3 velocity, const Quaternion spin)
{
    stateChanger_ = false;

    Effect::Set(position);
    node_->SetRotation(rotation);

    velocity_ = velocity;
    spin_ = spin;

    staticModel_->SetModel(model);
    staticModel_->SetMaterial(phaseMaterial_);
}
void Phaser::Update(float timeStep)
{
    Effect::Update(timeStep);

    node_->Translate(velocity_ * timeStep, TS_WORLD);
    node_->Rotate(spin_);

    float dissolveValue { Clamp(age_ * (2.3f + (spin_ == Quaternion::IDENTITY) * 2.3f), 0.0f, 1.0f) };
    phaseMaterial_->SetShaderParameter("Dissolve", dissolveValue);

    if (!stateChanger_) {

        if (dissolveValue >= 1.0f)
            Disable();

        velocity_   = velocity_.Lerp(Vector3::ZERO, timeStep + age_ * 0.23f);
        spin_       = spin_.Slerp(Quaternion::IDENTITY, timeStep + age_ * 0.23f);
    }

    if (age_ > 2.0f) {

        if (stateChanger_) {
            for (Controllable* c : GetSubsystem<InputMaster>()->GetControlled()) {

                if (c->IsEnabled())
                    return;
            }

            MC->SetGameState(GS_LOBBY);
        }

        Disable();
    }
}
