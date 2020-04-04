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

#include "apple.h"

#include "mirage.h"

void Apple::RegisterObject(Context *context)
{
    context->RegisterFactory<Apple>();
}

Apple::Apple(Context* context) : Pickup(context)
{

}

void Apple::OnNodeSet(Node *node)
{ if (!node) return;

    Pickup::OnNodeSet(node);

    node_->SetName("Apple");
    pickupType_ = PT_APPLE;
    initialPosition_ = Vector3::FORWARD * 10.8f;
    node_->SetPosition(initialPosition_);
    model_->SetModel(MC->GetModel("Apple"));
    model_->SetMaterial(MC->GetMaterial("GoldEnvmap"));

    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(0.55f, 0.55f, 0.23f, 0.55f), 0.42f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.8f));
    particleEmitter_->GetEffect()->SetColorFrames(colorFrames);

    graphicsNode_->CreateComponent<Mirage>()->SetColor(Color(0.6f, 0.5f, 0.23f, 0.9f));
}

void Apple::Update(float timeStep)
{
    Pickup::Update(timeStep);

    //Spin
    graphicsNode_->Rotate(Quaternion(0.0f, 100.0f * timeStep, 0.0f));
    //Float like a float
    float floatFactor = 0.5f - Min(0.5f, 0.5f * Abs(node_->GetPosition().y_));
    graphicsNode_->SetPosition(Vector3::UP * MC->Sine(0.23f, -floatFactor, floatFactor, 0.23f));
}
