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

#ifndef PHASER_H
#define PHASER_H

#include <Urho3D/Urho3D.h>

#include "effect.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Phaser : public Effect
{
public:
    Phaser(Context* context);

    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    virtual void Set(Model* model, const Vector3 position, const Vector3 velocity, const bool stateChanger = true, bool audible = true);
    virtual void Set(Model* model, const Vector3 position, const Quaternion rotation, const Vector3 velocity, const Quaternion spin);
private:
    StaticModel* staticModel_;
    SharedPtr<Material> phaseMaterial_;
    Vector3 velocity_;
    Quaternion spin_;
    bool stateChanger_;
};

#endif // PHASER_H
