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

#include "arena.h"

#include "mastercontrol.h"
#include "tile.h"

#include "explosion.h"
#include "brick.h"

void Arena::RegisterObject(Context *context)
{
    context->RegisterFactory<Arena>();
}

Arena::Arena(Context* context):
    LogicComponent(context),
    targetPosition_{ Vector3::UP * 0.666f },
    targetScale_{ Vector3::ONE * 0.05f },
    hexAffectors_{}
{
    SetUpdateEventMask(USE_UPDATE);
}

void Arena::OnNodeSet(Node *node)
{ if (!node) return;

    node_->SetPosition(targetPosition_);
    node_->SetScale(targetScale_);

    //Create hexagonal field
    //Lays a field of hexagons at the origin
    int bigHexSize{ ARENA_SIZE };
    for (int i{0}; i < bigHexSize; ++i) {
        for (int j{0}; j < bigHexSize; ++j) {
            if (i < (bigHexSize - bigHexSize / 4) + j / 2 &&                            //Exclude bottom right
                i > (bigHexSize / 4) - (j + 1) / 2 &&                                   //Exclude bottom left
                i + 1 < (bigHexSize - bigHexSize / 4) + ((bigHexSize - j + 1)) / 2 &&   //Exclude top right
                i - 1 > (bigHexSize / 4) - ((bigHexSize - j + 2) / 2)) {                //Exclude top left

                Vector3 tilePos{ (-bigHexSize / 2.0f + i) * 2.0f + j % 2,
                                 -0.1f,
                                 (-bigHexSize / 2.0f + j) * 1.8f};

                Node* tileNode{ node_->CreateChild("Tile", LOCAL) };
                tileNode->SetPosition(tilePos);
                tiles_.Push(tileNode->CreateComponent<Tile>());
            }
        }
    }
    //Add a directional light to the arena.
    Node* lightNode{ node_->CreateChild("Sun", LOCAL) };
    lightNode->SetPosition(Vector3::UP*5.0f);
    lightNode->SetRotation(Quaternion(90.0f, 0.0f, 0.0f));
    playLight_ = lightNode->CreateComponent<Light>();
    playLight_->SetLightType(LIGHT_DIRECTIONAL);
    playLight_->SetBrightness(0.8f);
    playLight_->SetRange(10.0f);
    playLight_->SetColor(Color(1.0f, 0.9f, 0.95f));
    playLight_->SetCastShadows(false);
    playLight_->SetEnabled(false);

    //Create heXon logo
    logoNode_ = node_->CreateChild("heXon", LOCAL);
    logoNode_->SetPosition(Vector3(0.0f, -4.0f, 0.0f));
    logoNode_->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
    logoNode_->SetScale(16.0f);
    StaticModel* logoModel{ logoNode_->CreateComponent<StaticModel>() };
    logoModel->SetModel(MC->GetModel("heXon"));
    logoMaterial_ = MC->GetMaterial("Loglow");
    xMaterial_ = MC->GetMaterial("X");
    logoModel->SetMaterial(0, logoMaterial_);
    logoModel->SetMaterial(1, xMaterial_);

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(Arena, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(Arena, EnterPlay));

    SubscribeToEvent(E_POSTUPDATE,  URHO3D_HANDLER(Arena, UpdateEffectVector));
}

void Arena::AddToAffectors(Node* affector)
{
    if (!hexAffectors_.Contains(affector))
        hexAffectors_.Insert(affector);
}
void Arena::RemoveFromAffectors(Node* affector)
{
    if (hexAffectors_.Contains(affector) )
        hexAffectors_.Erase(affector);
}

void Arena::UpdateEffectVector(StringHash, VariantMap&)
{
    effectVector_.Clear();
    for (Node* node : hexAffectors_) {

        if (node->IsEnabled()) {

            Pair<Vector3, float> pair{};
            pair.first_ = node->GetWorldPosition();
            pair.second_ = node->GetComponent<RigidBody>()->GetMass();

            if (node->HasComponent<Explosion>())
                pair.second_ *= 1.23f;

            pair.second_ = sqrt(pair.second_);

            effectVector_.Push(pair);
        }
    }
}

const PODVector<Pair<Vector3, float> >& Arena::GetEffectVector() const
{
    return effectVector_;
}

void Arena::EnterPlay(StringHash, VariantMap&)
{
    targetPosition_ = Vector3::DOWN * 0.23f;
    targetScale_ = Vector3::ONE;
    for (Tile* t : tiles_){
        t->lastOffsetY_ = 2.3f;
    }

    playLight_->SetEnabled(true);
}
void Arena::EnterLobby(StringHash, VariantMap&)
{
    targetPosition_ = Vector3::UP * 0.35f;
    targetScale_ = Vector3::ONE * 0.05f;

    playLight_->SetEnabled(false);
}

void Arena::Update(float timeStep)
{
    float lerpFactor{ MC->GetGameState() == GS_LOBBY ? 13.0f : 6.66f };
    float t{ Min(1.0f, timeStep * lerpFactor) };
    node_->SetPosition(node_->GetPosition().Lerp(targetPosition_, t));
    node_->SetScale(node_->GetScale().Lerp(targetScale_, pow(t, 0.88f) ));

    logoNode_->SetPosition(logoNode_->GetPosition().Lerp(MC->GetGameState() == GS_LOBBY
                                                         ? Vector3::UP * 4.0f * MC->Sine(0.666f, 0.23f, 1.23f)
                                                         : Vector3::UP * -4.0f, t));
    logoMaterial_->SetShaderParameter("MatDiffColor", logoMaterial_->GetShaderParameter("MatDiffColor").GetColor().Lerp(
                                          MC->GetGameState() == GS_LOBBY
                                          ? Color(0.42f, Random(0.666f), Random(0.666f), 2.0f) * MC->Sine(5.0f, 0.5f, 0.8f, 0.23f)
                                          : Color(0.0666f, 0.16f, 0.16f, 0.23f), t));
    logoMaterial_->SetShaderParameter("MatEmissiveColor", logoMaterial_->GetShaderParameter("MatEmissiveColor").GetColor().Lerp(
                                          MC->GetGameState() == GS_LOBBY
                                          ? Color(Random(0.42f), Random(0.42f), Random(0.42f)) * MC->Sine(4.0f, 1.3f, 2.3f, 0.23f)
                                          : Color(0.005f, 0.05f, 0.02f), t));
    xMaterial_->SetShaderParameter("MatDiffColor", MC->GetGameState() == GS_LOBBY
                                          ? logoMaterial_->GetShaderParameter("MatDiffColor").GetColor()
                                          : xMaterial_->GetShaderParameter("MatDiffColor").GetColor().Lerp(Color(0.0666f, 0.16f, 0.16f, 0.23f), t));
    xMaterial_->SetShaderParameter("MatEmissiveColor", MC->GetGameState() == GS_LOBBY
                                          ? logoMaterial_->GetShaderParameter("MatEmissiveColor").GetColor()
                                          : xMaterial_->GetShaderParameter("MatEmissiveColor").GetColor().Lerp(Color(0.005f, 0.05f, 0.02f), t));
}

Tile* Arena::GetRandomTile(bool forMason)
{
    if (tiles_.Size()) {
        Tile* tile{ nullptr };
        while (!tile) {

            Tile* tryTile{ tiles_[Random() * tiles_.Size()] };

            if (forMason) {

                for (float angle : {0.0f, 60.0f, 120.0f}) {
                    Vector3 axis{ Quaternion(angle, Vector3::UP) * Vector3::FORWARD };
                    float distanceToCenter{ Abs(tryTile->GetNode()->GetWorldPosition().ProjectOntoAxis(axis)) };
                    if ( distanceToCenter < 1.0f || distanceToCenter > 19.0f){
                        continue;
                    }
                }
            }

//            if (tryTile->IsFree()) {
                tile = tryTile;
//            }
        }
        return tile;
    }
    return nullptr;
}

void Arena::FlashX(Color color)
{
    xMaterial_->SetShaderParameter("MatEmissiveColor", color * 5.0f);
}
