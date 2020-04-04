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

#include "flash.h"

void Flash::RegisterObject(Context *context)
{
    context->RegisterFactory<Flash>();
}

Flash::Flash(Context* context):
    Effect(context),
    bigFlash_{CACHE->GetResource<ParticleEffect>("Particles/Flash.xml")},
    smallFlash_{CACHE->GetResource<ParticleEffect>("Particles/FlashSmall.xml")},
    initialBrightness_{2.0f}
{  

}

void Flash::OnNodeSet(Node *node)
{ if (!node) return;

    Effect::OnNodeSet(node);

    node_->SetName("Flash");

    particleEmitter_ = node_->CreateComponent<ParticleEmitter>();
    particleEmitter_->SetEffect(bigFlash_);

    light_ = node_->CreateComponent<Light>();
    light_->SetRange(10.0f);
    light_->SetColor(Color::WHITE);
    light_->SetBrightness(initialBrightness_);
}

void Flash::Update(float timeStep)
{
    Effect::Update(timeStep);
    light_->SetBrightness(Max(initialBrightness_ * (0.25f - age_) / 0.25f, 0.0f));
}

void Flash::Set(const Vector3 position, float gain, bool big)
{
    if (big && particleEmitter_->GetEffect() != bigFlash_){
        particleEmitter_->SetEffect(bigFlash_);
    } else if (!big && particleEmitter_->GetEffect() != smallFlash_){
        particleEmitter_->SetEffect(smallFlash_);
    }

    Effect::Set(position);

    if (gain > 0.0f) {

        PlaySample(MC->GetSample("Flash"), gain);
    }
}

void Flash::Disable()
{
    UnsubscribeFromEvent(E_POSTUPDATE);
    SceneObject::Disable();
}
