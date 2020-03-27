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

#include "heart.h"

#include "mirage.h"

void Heart::RegisterObject(Context *context)
{
    context->RegisterFactory<Heart>();
}

Heart::Heart(Context* context):
    Pickup(context)
{
}

void Heart::OnNodeSet(Node *node)
{ if (!node) return;

    Pickup::OnNodeSet(node);

    node_->SetName("Heart");
    pickupType_ = PT_HEART;
    initialPosition_ = Vector3::BACK * 10.8f;
    node_->SetPosition(initialPosition_);
    model_->SetModel(MC->GetModel("Heart"));
    model_->SetMaterial(MC->GetMaterial("RedEnvmap"));

    Vector<ColorFrame> colorFrames{};
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(0.75f, 0.23f, 0.23f, 0.42f), 0.23f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.42f));
    colorFrames.Push(ColorFrame(Color(0.75f, 0.23f, 0.23f, 0.75f), 0.75f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 1.2f));
    particleEmitter_->GetEffect()->SetColorFrames(colorFrames);

    graphicsNode_->CreateComponent<Mirage>()->SetColor(Color(0.7f, 0.23f, 0.1f, 0.75f));
}

void Heart::Update(float timeStep)
{
    Pickup::Update(timeStep);

    //Spin
    graphicsNode_->Rotate(Quaternion(0.0f, 100.0f * timeStep, 0.0f));
    //Float like a float
    float floatFactor = 0.5f - Min(0.5f, 0.5f * Abs(node_->GetPosition().y_));
    graphicsNode_->SetPosition(Vector3::UP * MC->Sine(0.23f, -floatFactor, floatFactor));
}
