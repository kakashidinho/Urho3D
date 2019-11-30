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

#ifndef BRICK_H
#define BRICK_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

class TailGenerator;

class Brick : public SceneObject
{
    URHO3D_OBJECT(Brick, SceneObject);
public:
    Brick(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Set(Vector3 position, Vector3 direction);
    void Disable() override;
    void HandleTriggerStart(StringHash, VariantMap&);
    void Update(float timeStep) override;
    void FixedPostUpdate(float timeStep);
private:
    RigidBody* rigidBody_;
    ParticleEmitter* particleEmitter_;
    SharedPtr<Material> spikeMaterial_;
    CollisionShape* trigger_;
    float damage_;
    float traveled_;
};

#endif // BRICK_H
