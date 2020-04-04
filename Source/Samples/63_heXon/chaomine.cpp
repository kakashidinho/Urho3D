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

#include "chaomine.h"

#include "spawnmaster.h"
#include "player.h"
#include "chaozap.h"

void ChaoMine::RegisterObject(Context *context)
{
    context->RegisterFactory<ChaoMine>();
}

ChaoMine::ChaoMine(Context* context) : Enemy(context),
    colorSet_{0}
{
}

void ChaoMine::OnNodeSet(Node *node)
{
    Enemy::OnNodeSet(node);

    if (!node)
        return;

    node_->SetName("ChaoMine");
    big_ = false;

    rigidBody_->SetMass(0.5f);
    //Overrides Enemy values
    meleeDamage_ = 0.1f;
    initialHealth_ = 0.05f;
    worth_ = 1;

    countDown_ = Random(1.0f, 5.0f);
    innerNode_ = node_->CreateChild();
    innerModel_ = innerNode_->CreateComponent<StaticModel>();
    innerModel_->SetModel(MC->GetModel("MineInner"));
    innerModel_->SetMaterial(0, MC->GetMaterial("GreenEnvmap"));

    outerNode_ = node_->CreateChild();
    outerModel_ = outerNode_->CreateComponent<StaticModel>();
    outerModel_->SetModel(MC->GetModel("MineOuter"));
    outerModel_->SetMaterial(0, MC->GetMaterial("GreenGlowEnvmap"));
    outerModel_->SetMaterial(1, MC->GetMaterial("GreenEnvmap"));
}

void ChaoMine::Set(const Vector3 position, int colorSet)
{
    colorSet_ = colorSet;

    innerModel_->SetMaterial(0, MC->colorSets_[colorSet_].hullMaterial_);
    outerModel_->SetMaterial(0, MC->colorSets_[colorSet_].glowMaterial_);
    outerModel_->SetMaterial(1, MC->colorSets_[colorSet_].hullMaterial_);

    Enemy::Set(position);
}

void ChaoMine::Update(float timeStep)
{
    Enemy::Update(timeStep);

    //Spin
    innerNode_->Rotate(Quaternion(50.0f*timeStep, 80.0f*timeStep, 92.0f*timeStep));
    outerNode_->Rotate(Quaternion(-60.0f*timeStep,-101.0f*timeStep, -95.0f*timeStep));
}

void ChaoMine::CheckHealth()
{
    if (node_->IsEnabled() &&
       (health_ <= 0 || panicTime_ > 23.0f)) {
        ChaoZap* chaoZap{ GetSubsystem<SpawnMaster>()->Create<ChaoZap>() };
        Enemy::CheckHealth();
        chaoZap->Set(GetPosition(), colorSet_);
    }
}

void ChaoMine::HandleNodeCollision(StringHash, VariantMap& eventData)
{
    Node* otherNode{ static_cast<Node*>(eventData[NodeCollision::P_OTHERNODE].GetPtr()) };

    Enemy* e{ otherNode->GetDerivedComponent<Enemy>() };

    if (e && !e->IsInstanceOf<ChaoMine>()){
        SetHealth(0.0f);
    }
}
