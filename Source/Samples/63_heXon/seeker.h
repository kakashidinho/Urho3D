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

#ifndef SEEKER_H
#define SEEKER_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

class TailGenerator;

class Seeker : public SceneObject
{
    friend class ChaoFlash;
    friend class ChaoZap;
    URHO3D_OBJECT(Seeker, SceneObject);
public:
    Seeker(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void FixedUpdate(float timeStep) override;

    void HandleTriggerStart(StringHash eventType, VariantMap &eventData);
    void Set(Vector3 position, bool sound = true);
    void Disable();

    void SetLinearVelocity(const Vector3& velocity);
protected:
    RigidBody* rigidBody_;
    Node* target_;
//    RibbonTrail* tailGen_;
    Pair<TailGenerator*, TailGenerator*> tailGens_;

    float age_;
    float lifeTime_;
    float damage_;
private:
    void AddTails();
    void RemoveTails();
    Vector3 TargetPosition();
};

#endif // SEEKER_H
