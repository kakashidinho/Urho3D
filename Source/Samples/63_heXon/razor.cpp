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

#include "razor.h"

#include "mirage.h"
#include "phaser.h"
#include "spawnmaster.h"

void Razor::RegisterObject(Context *context)
{
    context->RegisterFactory<Razor>();
}

Razor::Razor(Context* context):
    Enemy(context),
    topSpeed_{10.0f},
    aimSpeed_{0.25f * topSpeed_},
    spinRate_{}
{
    sprite_ = true;
    meleeDamage_ = 0.9f;
}

void Razor::OnNodeSet(Node* node)
{ if (!node) return;

    Enemy::OnNodeSet(node);

    if (!sprite_) {

        SharedPtr<Material> black{ MC->GetMaterial("Razor")->Clone() };
        topNode_ = node_->CreateChild("RazorTop");
        topModel_ = topNode_->CreateComponent<StaticModel>();
        topModel_->SetModel(MC->GetModel("RazorHalf"));
        topModel_->SetMaterial(0, MC->GetMaterial("Razor"));
        topModel_->SetMaterial(1, centerModel_->GetMaterial());

        bottomNode_ = node_->CreateChild("RazorBottom");
        bottomNode_->SetRotation(Quaternion(180.0f, Vector3::RIGHT));
        bottomModel_ = bottomNode_->CreateComponent<StaticModel>();
        bottomModel_->SetModel(MC->GetModel("RazorHalf"));
        bottomModel_->SetMaterial(0, black);
        bottomModel_->SetMaterial(1, centerModel_->GetMaterial());

    } else {

        AnimatedBillboardSet* sprite{ node_->CreateComponent<AnimatedBillboardSet>() };
        SharedPtr<Material> mat{ CACHE->GetResource<Material>("Materials/RazorSprite.xml")->Clone() };

        sprite->SetNumBillboards(1);
        sprite->SetMaterial(mat);
        sprite->SetSorted(true);
        sprite->SetRelative(true);
        sprite->SetSpeed(10.0f);

        for (Billboard& bb : sprite->GetBillboards()) {

            bb.size_ = Vector2(1.17f, 1.17f);
            bb.enabled_ = true;
        }

        sprite->LoadFrames(CACHE->GetResource<XMLFile>("Textures/Mirage.xml"));
        sprite->Commit();
    }

    rigidBody_->SetLinearRestThreshold(0.0023f);

    Mirage* mirage{ node_->CreateComponent<Mirage>() };
    mirage->SetColor(color_ * 0.9f);
    mirage->SetSize(0.9f);
}
void Razor::Set(Vector3 position)
{
    aimSpeed_ = 0.25f * topSpeed_;
    Enemy::Set(position);
}
void Razor::Update(float timeStep)
{
    if (!node_->IsEnabled())
        return;

    Enemy::Update(timeStep);

    //Spin
    spinRate_ = Max(5.0f, 55.0f * aimSpeed_ - 17.0 * rigidBody_->GetLinearVelocity().Length()) ;

    if (!sprite_) {

        topNode_->Rotate(Quaternion(0.0f, timeStep * spinRate_, 0.0f));
        bottomNode_->Rotate(Quaternion(0.0f, timeStep * spinRate_, 0.0f));
        //Pulse
        topModel_->GetMaterial(0)->SetShaderParameter("MatEmissiveColor", GetGlowColor());

    } else {

        smokeNode_->SetScale(0.75f);

        AnimatedBillboardSet* sprite{ GetComponent<AnimatedBillboardSet>() };
        sprite->GetMaterial()->SetShaderParameter("MatEmissiveColor", (color_ + GetGlowColor()) * 0.42f);
        sprite->SetSpeed(spinRate_ * 0.042f);
        sprite->GetBillboard(0)->size_ = Vector2(1.23f, 1.23f + 0.023f * Max(0.0f, node_->GetWorldPosition().z_ / -5.0f));
        sprite->Commit();
    }
}

void Razor::FixedUpdate(float timeStep)
{
    //Get moving
    if (rigidBody_->GetLinearVelocity().Length() < rigidBody_->GetLinearRestThreshold() && IsEmerged()) {
        rigidBody_->ApplyForce(timeStep * 42.0f * (Quaternion(Random(6) * 60.0f, Vector3::UP) * Vector3::FORWARD));
    }
    //Adjust speed
    else if (rigidBody_->GetLinearVelocity().Length() < aimSpeed_) {
        rigidBody_->ApplyForce(timeStep * 235.0f * rigidBody_->GetLinearVelocity().Normalized() * Max(aimSpeed_ - rigidBody_->GetLinearVelocity().Length(), 0.1f));
    }
    else {
        float overSpeed{ rigidBody_->GetLinearVelocity().Length() - aimSpeed_ };
        rigidBody_->ApplyForce(timeStep * 100.0f * -rigidBody_->GetLinearVelocity() * overSpeed);
    }

    Enemy::FixedUpdate(timeStep);
}

void Razor::Hit(float damage, int ownerID)
{
    Enemy::Hit(damage, ownerID);
    aimSpeed_ = (0.25f + 0.75f * panic_) * topSpeed_;
}
void Razor::Blink(Vector3 newPosition)
{
    if (!sprite_) {

        for (StaticModel* sm: {topModel_, bottomModel_}) {

            Phaser* phaser{ SPAWN->Create<Phaser>() };
            phaser->Set(sm->GetModel(), GetPosition(), node_->GetRotation(), rigidBody_->GetLinearVelocity(), Quaternion(spinRate_, Vector3::UP));
        }
    }

    SceneObject::Blink(newPosition);
}
