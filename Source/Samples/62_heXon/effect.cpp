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

#include "effect.h"

Effect::Effect(Context* context):
    SceneObject(context),
    age_{0.0f},
    emitTime_{0.1f}
{
}

void Effect::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

    blink_ = false;

    particleEmitter_ = node_->CreateComponent<ParticleEmitter>();
}

void Effect::Update(float timeStep)
{
    age_ += timeStep;

    bool livingParticles{ true };

    ParticleEffect* particleEffect{ particleEmitter_->GetEffect() };
    if (particleEffect) {
        if (age_ > emitTime_ + particleEffect->GetMaxTimeToLive()) {
            livingParticles = false;
        } else if (age_ > emitTime_) {
            particleEmitter_->SetEmitting(false);
        }
    }

    if (!livingParticles && !IsPlayingSound())
        Disable();
}

void Effect::Set(const Vector3 position)
{
    SceneObject::Set(position);
    age_ = 0.0f;
    ParticleEffect* particleEffect{ particleEmitter_->GetEffect() };
    if (particleEffect) {

        particleEmitter_->RemoveAllParticles();
        particleEmitter_->SetEmitting(true);
    }
}
