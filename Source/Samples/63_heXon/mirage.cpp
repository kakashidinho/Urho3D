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


#include "mirage.h"

#include "arena.h"

void Mirage::RegisterObject(Context* context)
{
    context->RegisterFactory<Mirage>();
}

Mirage::Mirage(Context* context) : AnimatedBillboardSet(context),
    billboardSet_{},
    color_{0.42f, 0.42f, 0.42f, 0.42f},
    size_{1.0f},
    fade_{1.0f}
{
}

void Mirage::OnNodeSet(Node* node)
{ if(!node) return;

    SetNumBillboards(2);
    SetMaterial(CACHE->GetResource<Material>("Materials/Mirage.xml"));
    SetSorted(true);
    SetRelative(true);

    for (Billboard& bb : GetBillboards()) {

//        bb.size_ = Vector2::ONE;
//        bb.position_ = Vector3::ZERO;
        bb.color_ = Color::TRANSPARENT_BLACK;
        bb.enabled_ = true;

    }

    LoadFrames(CACHE->GetResource<XMLFile>("Textures/Mirage.xml"));
    Commit();

}

void Mirage::UpdateGeometry(const FrameInfo& frame)
{
    Vector3 normalizedPosition{ node_->GetWorldPosition().Normalized() };

    Pair<Vector3, Vector3> hexants(-MC->GetHexant(Quaternion(-30.0f, Vector3::UP) * normalizedPosition),
                                   -MC->GetHexant(Quaternion( 30.0f, Vector3::UP) * normalizedPosition)
                                  );

    for (unsigned b{0}; b < GetNumBillboards(); ++b) {

        Billboard& bb{ GetBillboards()[b] };

        float radius{ ARENA_RADIUS };
        Vector3 offset{ (b == 0 ? hexants.first_ : hexants.second_) * 2.0f * radius };
        bb.position_ = (Vector3::ONE / node_->GetWorldScale()) * (node_->GetWorldRotation().Inverse() * (offset + (node_->GetWorldPosition() + offset) * 0.005f));
        float intensity{ Clamp(1.0f - ((node_->GetWorldPosition() + offset).ProjectOntoAxis(offset.Normalized()) - radius) * 0.3f, 0.0f, 1.0f) };

        if (hexants.first_ == hexants.second_ && b > 0)
            intensity = 0.0f;

        bb.color_ = intensity * intensity * color_;
        float squish{ intensity * Clamp(node_->GetWorldPosition().z_ / radius, 0.0f, 1.0f) };
        bb.size_ = size_ * Vector2::ONE * (1.5f - 0.5f * intensity) - 0.23f * Vector2::UP * squish * squish * squish * squish;
    }

    bool enabled{ node_->IsEnabled() };
    if (IsEnabled() != enabled)
        SetEnabled(enabled);

    AnimatedBillboardSet::UpdateGeometry(frame);
}
