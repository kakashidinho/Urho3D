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

#include "bubble.h"

StaticModelGroup* Bubble::bubbleGroup_{};

void Bubble::RegisterObject(Context *context)
{
    context->RegisterFactory<Bubble>();

}

Bubble::Bubble(Context* context):
    Effect(context),
    spinAxis_{Vector3(Random(), Random(), Random()).Normalized()},
    spinVelocity_{LucKey::RandomSign() * Random(23.0f, 42.0f)},
    baseScale_{Random(0.25f, 0.95f)}
{
}

void Bubble::OnNodeSet(Node *node)
{ if (!node) return;

    Effect::OnNodeSet(node_);

    baseScale_ *= baseScale_;
    node_->SetName("Bubble");

    if (!bubbleGroup_) {
        bubbleGroup_ = MC->scene_->CreateComponent<StaticModelGroup>();
        bubbleGroup_->SetModel(MC->GetModel("Box"));
        bubbleGroup_->SetMaterial(MC->GetMaterial("Bubble"));
    }
}

void Bubble::Set(const Vector3 position)
{
    bubbleGroup_->AddInstanceNode(node_);

    Effect::Set(position);
}

void Bubble::Update(float timeStep)
{
    if (node_->GetPosition().y_ > 42.0f)
        Disable();

    node_->Translate(Vector3::UP * timeStep * (6.66f + MC->SinceLastReset() * 0.0023f), TS_WORLD);
    node_->Rotate(Quaternion(timeStep * spinVelocity_, spinAxis_));
    node_->SetWorldScale(Vector3(MC->Sine(2.0f - baseScale_, baseScale_ * 0.88f, baseScale_ * 1.23f, spinVelocity_),
                                 MC->Sine(3.0f - baseScale_, baseScale_ * 0.88f, baseScale_ * 1.23f, spinVelocity_ + 2.0f),
                                 MC->Sine(2.3f - baseScale_, baseScale_ * 0.88f, baseScale_ * 1.23f, spinVelocity_ + 3.0f)));
}

void Bubble::Disable()
{
    bubbleGroup_->RemoveInstanceNode(node_);
    SceneObject::Disable();
}
