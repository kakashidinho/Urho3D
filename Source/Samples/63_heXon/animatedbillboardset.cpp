/* OG Tatt
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

#include "animatedbillboardset.h"

void AnimatedBillboardSet::RegisterObject(Context* context)
{
    context->RegisterFactory<AnimatedBillboardSet>();
}

AnimatedBillboardSet::AnimatedBillboardSet(Context* context) : BillboardSet(context),
    textureFrames_{},
    texIndices_{},
    animationTimers_{},
    synced_{ true },
    speed_{ 1.0f }
{
}

void AnimatedBillboardSet::LoadFrames(XMLFile* file)
{
    textureFrames_.Clear();

    XMLElement root{ file->GetRoot() };
    XMLElement anim{ root.GetChild("texanim") };

    while (anim) {

        TextureFrame frame{};
        frame.uv_ = anim.GetRect("uv");
        frame.time_ = anim.GetFloat("time");
        textureFrames_.Push(frame);

        anim = anim.GetNext("texanim");
    }

    if (textureFrames_.Empty())
        return;

    for (unsigned b{0}; b < GetNumBillboards(); ++b) {

        texIndices_[b] = 0;
        animationTimers_[b] = textureFrames_.At(texIndices_[b]).time_;

        GetBillboard(b)->uv_ = textureFrames_[texIndices_[b]].uv_;
    }
    Commit();
}

void AnimatedBillboardSet::UpdateGeometry(const FrameInfo &frame)
{
    bool sceneUpdate{ false };

    if (node_->GetScene())
        sceneUpdate = node_->GetScene()->IsUpdateEnabled();

    if (!textureFrames_.Size() || !sceneUpdate)
        return;

    bool frameStepped{ false };
    for (unsigned b{0}; b < GetNumBillboards(); ++b) {

        if (!GetBillboard(b)->enabled_ && (!synced_ || b != 0))
            continue;

        // Texture animation
        unsigned& texIndex{ texIndices_[synced_ ? 0 : b] };
        float& time{ animationTimers_[synced_ ? 0 : b] };
        if (!synced_ || b == 0) {

            time += frame.timeStep_ * speed_;
        }


        while (time >= textureFrames_[texIndex].time_)
        {
            if (!synced_ || b == 0) {

                ++texIndex;
                if (texIndex >= textureFrames_.Size()) {
                    texIndex = 0;
                    time -= textureFrames_[textureFrames_.Size() - 1].time_;
                }
                frameStepped = true;
            }
        }

        if (frameStepped)
            GetBillboard(b)->uv_ = textureFrames_[texIndex].uv_;
    }
    Commit();

    BillboardSet::UpdateGeometry(frame);
}
