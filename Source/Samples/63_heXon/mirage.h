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


#ifndef MIRAGE_H
#define MIRAGE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"
#include "animatedbillboardset.h"

class Mirage : public AnimatedBillboardSet
{
    URHO3D_OBJECT(Mirage, AnimatedBillboardSet);
public:
    Mirage(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    void SetColor(const Color color) { color_ = color; }
    void SetSize(const float size) { size_ = size; }

    void UpdateGeometry(const FrameInfo& frame);
private:
    AnimatedBillboardSet* billboardSet_;
    Color color_;
    float size_;
    float fade_;
};

#endif // MIRAGE_H
