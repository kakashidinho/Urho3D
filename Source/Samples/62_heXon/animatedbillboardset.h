/* HoverAce
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

#ifndef ANIMATEDBILLBOARDSET_H
#define ANIMATEDBILLBOARDSET_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include "luckey.h"

class AnimatedBillboardSet : public BillboardSet
{
    URHO3D_OBJECT(AnimatedBillboardSet, BillboardSet);
public:
    static void RegisterObject(Context* context);
    AnimatedBillboardSet(Context* context);
    void UpdateGeometry(const FrameInfo& frame) override;
    void LoadFrames(XMLFile* file);

    void SetSpeed(const float speed) { speed_ = speed; }
private:
    Vector<TextureFrame> textureFrames_;
    HashMap<unsigned, unsigned> texIndices_;
    HashMap<unsigned, float> animationTimers_;

    bool synced_;
    float speed_;
};

#endif // ANIMATEDBILLBOARDSET_H
