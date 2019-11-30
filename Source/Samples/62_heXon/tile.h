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

#ifndef TILE_H
#define TILE_H

#include <Urho3D/Urho3D.h>

#include "arena.h"

namespace Urho3D {
class Node;
}

class Tile : public LogicComponent
{
    URHO3D_OBJECT(Tile, LogicComponent);
    friend class Arena;
    friend class InputMaster;
    friend class SpawnMaster;
public:
    Tile(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
private:
    void Update(float timeStep) override;

    Vector3 referencePosition_;
    float centerDistExp_;
    float wave_;
    float lastOffsetY_;
    StaticModel* model_;
    bool flipped_;
};

#endif // TILE_H
