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

#include "hitfx.h"

void HitFX::RegisterObject(Context *context)
{
    context->RegisterFactory<HitFX>();

    MC->GetSample("Hit");
}

HitFX::HitFX(Context* context):
    Effect(context)
//    sample_{MC->GetSample("Hit")}
{
}

void HitFX::OnNodeSet(Node *node)
{ if (!node) return;

    Effect::OnNodeSet(node);

}

void HitFX::Set(const Vector3 position, int colorSet, const bool sound)
{
    particleEmitter_->SetEffect(MC->colorSets_[colorSet].hitFx_);
    Effect::Set(position);

    if (sound)
        PlaySample(MC->GetSample("Hit"), 0.1f);
}
