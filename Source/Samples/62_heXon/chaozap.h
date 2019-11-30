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

#ifndef CHAOZAP_H
#define CHAOZAP_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "sceneobject.h"

class ChaoZap : public SceneObject
{
    friend class SpawnMaster;
    URHO3D_OBJECT(ChaoZap, SceneObject);
public:
    ChaoZap(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep) override;
    void Set(const Vector3 position, int colorSet);
    void OnNodeSet(Node* node) override;
protected:
    void Disable();
private:
    const float size_;
    RigidBody* rigidBody_;
    StaticModel* chaoModel_;
    SharedPtr<Material> chaoMaterial_;
    Vector<Sound*> samples_;
};

#endif // CHAOZAP_H
