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
#include "effectmaster.h"
#include "brick.h"

#include "mason.h"

void Mason::RegisterObject(Context* context)
{
    context->RegisterFactory<Mason>();

    MC->GetSample("Brick");
}

Mason::Mason(Context* context) : Enemy(context),
    shots_{0},
    sinceShot_{0.0f},
    shotInterval_{0.23f},
    spinInterval_{2.3f},
    toSpin_{0.0f},
    spun_{0.0f}
{

}

void Mason::OnNodeSet(Node* node)
{ if (!node) return;


    Enemy::OnNodeSet(node);

    meleeDamage_ = 0.23f;

    health_ = initialHealth_ = 15.0f;
    worth_  = 42;

    rigidBody_->SetMass(3.4f);
    rigidBody_->SetLinearFactor(Vector3::ZERO);

    blackMaterial_ = MC->GetMaterial("Razor")->Clone();
    glowMaterial_  = MC->GetMaterial("Razor")->Clone();

    glowMaterial_->SetShaderParameter("MatEmissiveColor", color_);

    topNode_ = node_->CreateChild("MasonTop");
    topNode_->Rotate(Quaternion(30.0f, Vector3::UP));
    StaticModel* topModel{ topNode_->CreateComponent<StaticModel>() };
    topModel->SetModel(MC->GetModel("MasonTop"));
    topModel->SetMaterial(0, blackMaterial_);
    topModel->SetMaterial(1, glowMaterial_);

    bottomNode_ = node_->CreateChild("MasonBottom");
    bottomNode_->Rotate(Quaternion(30.0f, Vector3::UP));
    StaticModel* bottomModel_{ bottomNode_->CreateComponent<StaticModel>() };
    bottomModel_->SetModel(MC->GetModel("MasonBottom"));
    bottomModel_->SetMaterial(0, blackMaterial_);
    bottomModel_->SetMaterial(1, glowMaterial_);
}

void Mason::Set(const Vector3 position)
{
    Enemy::Set(position);

    shots_          = 0;
    sinceShot_      = 0.0f;
    shotInterval_   = 0.23f;
    spinInterval_   = 2.3f;
    toSpin_         = 0.0f;
    spun_           = 0.0f;

    float rot{ Random(3) * 60.0f + 30.0f };

    topNode_->SetRotation(Quaternion(rot, Vector3::UP));
    bottomNode_->SetRotation(Quaternion(rot, Vector3::UP));
}

void Mason::Update(float timeStep)
{
    if (!node_->IsEnabled()) return;

    Enemy::Update(timeStep);

    blackMaterial_->SetShaderParameter("MatEmissiveColor", GetGlowColor());

    if (!IsEmerged())
        return;

    sinceShot_ += timeStep;

    if (sinceShot_ > shotInterval_) {
        if (shots_ != 0 && shots_ < MaxShots())
            Shoot();
        else
            shots_ = 0;
    }

    if (sinceShot_ > spinInterval_) {

        if (toSpin_ == 0.0f) {
            shots_ = 0;
            spun_ = 0.0f;

            toSpin_ = 360.0f * Random(5, 8 - (int)(panic_ * 3))
                     + 60.0f * Random(3);
        }

        float spinVelocity = 666.0f;

        if (Min(spun_, toSpin_) < 180.0f) {

            spinVelocity *= 0.01f + Min(spun_, toSpin_) / 180.0f;
        }

        float rotDelta{ Min(spinVelocity * timeStep, toSpin_) };
        toSpin_ -= rotDelta;
        spun_ += rotDelta;

        topNode_->Rotate(Quaternion(rotDelta, Vector3::UP));
        bottomNode_->Rotate(Quaternion(2.0f * rotDelta, Vector3::DOWN));

        if (toSpin_ == 0.0f) {

            shotInterval_ = 0.23f - panic_ * 0.1f;
            Shoot();

        }
    }
}

void Mason::Shoot()
{
    PlaySample(MC->GetSample("Brick_s"), 0.14f, false);

    GetSubsystem<SpawnMaster>()->Create<Brick>()->Set(GetPosition(), topNode_->GetDirection());
    GetSubsystem<SpawnMaster>()->Create<Brick>()->Set(GetPosition(), -topNode_->GetDirection());

    sinceShot_ = 0.0f;
    ++shots_;

    PlaySample(MC->GetSample("Brick"), 0.34f);
}

int Mason::MaxShots()
{
    return (int)(panic_ * 10) + 3;
}




