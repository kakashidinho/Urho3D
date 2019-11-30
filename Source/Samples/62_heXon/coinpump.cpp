/* Edddy
// Copyright (C) 2019 LucKey Productions (luckeyproductions.nl)
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

#include "spawnmaster.h"
#include "coin.h"

#include "coinpump.h"

void CoinPump::RegisterObject(Context* context)
{
    context->RegisterFactory<CoinPump>();
}

CoinPump::CoinPump(Context* context) : Enemy(context),
    sinceHit_{0.55f}
{
}

void CoinPump::OnNodeSet(Node* node)
{ if (!node) return;

    Enemy::OnNodeSet(node);

    health_ = initialHealth_ = 10.0f;
    worth_ = 0;
    meleeDamage_ = 0.0f;

    rigidBody_->SetMass(0.55f);
    rigidBody_->SetLinearFactor(Vector3::ZERO);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(0.55f);
    rigidBody_->SetFriction(0.1f);

    AnimatedModel* pumpModel{ node_->CreateComponent<AnimatedModel>() };
    pumpModel->SetModel(MC->GetModel("CoinPump"));
    pumpModel->SetMaterial(MC->GetMaterial("GoldEnvmap"));
}

void CoinPump::Update(float timeStep)
{
    Emerge(timeStep);
    sinceHit_ += timeStep;
    node_->GetComponent<AnimatedModel>()->SetMorphWeight(0, Clamp(0.55f - sinceHit_, 0.0f, 1.0f));
}

void CoinPump::Hit(float damage, const int colorSet)
{
    damage = 1.0f;

    if (health_ > 0.0f) {

        if (sinceHit_ > 0.23f){

            Enemy::Hit(damage, colorSet);
            rigidBody_->ApplyTorqueImpulse(.55f * Vector3::UP);
            Coin* coin{ GetSubsystem<SpawnMaster>()->Create<Coin>() };
            coin->Set(GetPosition());
            coin->Launch();
            sinceHit_ = 0.0f;
        }

    } else
        Explode();
}
