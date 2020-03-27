/* heXon
// Copyright (C) 2018 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
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

#include "inputmaster.h"
#include "controllable.h"

Controllable::Controllable(Context* context) : SceneObject(context),
    path_{},
    move_{},
    aim_{},
    maxPitch_{90.0f},
    minPitch_{0.0f},

    actions_{},
    actionSince_{},
    untilThought_{},
    model_{},
    rigidBody_{},
    collisionShape_{},
    animCtrl_{}
{
    for (int a{0}; a < NUM_ACTIONS; ++a)
        actionSince_[a] = 0.0f;
}
void Controllable::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node_);

    model_ = node_->CreateComponent<AnimatedModel>();
    rigidBody_ = node_->CreateComponent<RigidBody>();
    collisionShape_ = node_->CreateComponent<CollisionShape>();
    animCtrl_ = node_->CreateComponent<AnimationController>();

    model_->SetCastShadows(true);

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(Controllable, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(Controllable, EnterPlay));
}
void Controllable::Update(float timeStep)
{
    if (GetPlayer() == nullptr)
        return;

    if (GetPlayer()->IsHuman() && !path_.Size()) {

        for (unsigned a{0}; a < actions_.size(); ++a){

            if (actions_[a])
                actionSince_[a] += timeStep;
        }
    } else {
        untilThought_ -= timeStep;
        if (untilThought_ < 0.0f) {
            untilThought_ = Random(0.023f, 0.1f);

            Think();
        }
    }
}

void Controllable::SetMove(Vector3 move)
{
    move = move.Normalized() * Pow(move.Length() * 1.05f, 2.0f);

    if (move.Length() > 1.0f)
        move.Normalize();

    move_ = move;
}

void Controllable::SetAim(Vector3 aim)
{
    aim = aim * (Vector3::ONE - Vector3::UP);

    aim_ = aim.Normalized();
}

void Controllable::SetActions(ControllableActions actions)
{
    if (actions == actions_)
        return;
    else
        for (unsigned i{0}; i < actions.size(); ++i){

            if (actions[i] != actions_[i]){
                actions_[i] = actions[i];

                if (actions[i])
                    HandleAction(i);
            }
        }
}
void Controllable::HandleAction(int actionId)
{
    actionSince_[actionId] = 0.0f;
}

void Controllable::AlignWithMovement(float timeStep)
{
    Quaternion rot{ node_->GetRotation() };
    Quaternion targetRot{};
    Vector3 direction{ 0.13f * move_ + rigidBody_->GetLinearVelocity() * Vector3(1.0f, 0.0f, 1.0f)};
    if (direction.Length() < 0.1f)
        return;

    targetRot.FromLookRotation(direction);
    rot = rot.Slerp(targetRot, Clamp(timeStep * 5.0f, 0.0f, 1.0f));
    node_->SetRotation(rot);
}

void Controllable::ClampPitch(Quaternion& rot)
{
    float maxCorrection{rot.EulerAngles().x_ - maxPitch_};
    if (maxCorrection > 0.0f)
        rot = Quaternion(-maxCorrection, node_->GetRight()) * rot;

    float minCorrection{rot.EulerAngles().x_ - minPitch_};
    if (minCorrection < 0.0f)
        rot = Quaternion(-minCorrection, node_->GetRight()) * rot;
}

void Controllable::ClearControl()
{
    path_.Clear();
    ResetInput();
}

void Controllable::Think()
{
    if (!path_.Size())
        return;

    else {
        float toNextNode{ node_->GetPosition().DistanceToPoint(path_[0]) };

        if (toNextNode < (0.1f + 0.34f * (path_.Size() > 1))) {

            path_.Erase(0);

        } else

            SetMove((path_[0] - node_->GetPosition()).Normalized() * Clamp(toNextNode * 2.3f, 0.1f, 1.0f));
    }
}

Player* Controllable::GetPlayer()
{
    return GetSubsystem<InputMaster>()->GetPlayerByControllable(this);
}
