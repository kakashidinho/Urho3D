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

#ifndef BULLET_H
#define BULLET_H

#include "sceneobject.h"

#include <Urho3D/Urho3D.h>

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Bullet : public SceneObject
{
    friend class Ship;
    friend class SpawnMaster;
    URHO3D_OBJECT(Bullet, SceneObject);
public:
    Bullet(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Set(const Vector3 position, const int playerId, const Vector3 direction, Vector3 force, const float damage);

    void FixedUpdate(float timeStep) override;

    int GetPlayerID() const noexcept { return colorSet_; }
protected:
    SharedPtr<RigidBody> rigidBody_;

    void Update(float timeStep) override;
private:
    static HashMap<int, StaticModelGroup*> bulletGroups_;

    int colorSet_;
    float age_;
    float timeSinceHit_;
    float lifeTime_;
    bool fading_;
    float damage_;
    void HitCheck(const float timeStep);
    void Disable();
};

#endif // BULLET_H
