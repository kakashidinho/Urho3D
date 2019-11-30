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

#include "baphomech.h"

void Baphomech::RegisterObject(Context* context)
{
    context->RegisterFactory<Baphomech>();
}

Baphomech::Baphomech(Context* context) : Enemy(context),
    eyes_{}
{
}

void Baphomech::OnNodeSet(Node* node)
{ if(!node) return;

    Enemy::OnNodeSet(node);

    meleeDamage_ = 2.3f;

    health_ = initialHealth_ = 420.0f;
    worth_ = 666;

    StaticModel* head{ node_->CreateComponent<StaticModel>() };
    head->SetModel(MC->GetModel("BaphomechHead"));
    head->SetMaterial(0, MC->GetMaterial("Metal"));
    head->SetMaterial(1, MC->GetMaterial("Spire"));
    head->SetMaterial(2, MC->GetMaterial("Chaosphere"));
    head->SetMaterial(3, MC->GetMaterial("Spire"));

    rigidBody_->SetMass(23.0f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(2.3f);

    node_->GetComponent<CollisionShape>()->SetSphere(6.0f);

}

void Baphomech::FixedUpdate(float timeStep)
{
    Player* player{ MC->GetNearestPlayer(node_->GetWorldPosition()) };
    if (!player)
        return;

    Vector3 playerPos{ player->GetPosition() };
    Vector3 angularVelocity{ rigidBody_->GetAngularVelocity() };

    rigidBody_->ApplyTorque(((-node_->GetRight()).Angle(node_->GetWorldPosition() - playerPos) + Pow(angularVelocity.y_, 5.0f) * 420.0f) * Vector3::UP   * timeStep * 23.0f);
    rigidBody_->ApplyTorque(  (node_->GetRight().Angle( node_->GetWorldPosition() - playerPos) + Pow(angularVelocity.y_, 5.0f) * 420.0f) * Vector3::DOWN * timeStep * 23.0f);
}



