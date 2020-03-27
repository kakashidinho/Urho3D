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

#include "highest.h"
#include "pilot.h"


void Highest::RegisterObject(Context *context)
{
    context->RegisterFactory<Highest>();
}

Highest::Highest(Context* context) : LogicComponent(context),
    highestScore_{0}
{
}

void Highest::OnNodeSet(Node *node)
{ if (!node) return;

    node_->SetPosition(HIGHEST_POS);
    node_->Rotate(Quaternion(45.0f, Vector3::RIGHT));
    node_->Rotate(Quaternion(180.0f, Vector3::UP));
    Node* podiumNode{ node_->CreateChild("Podium") };
    podiumNode->SetScale(0.5f);
    StaticModel* hexPodium{ podiumNode->CreateComponent<StaticModel>() };
    hexPodium->SetModel(MC->GetModel("Hexagon"));
    hexPodium->SetMaterial(MC->GetMaterial("BackgroundTile")->Clone());
    Node* highestPilotNode{ podiumNode->CreateChild("HighestPilot") };
    highestPilotNode->SetScale(2.0f);

    Node* spotNode{ node_->CreateChild("HighestSpot") };
    spotNode->Translate(Vector3(0.0f, -2.0f, 3.0f));
    spotNode->LookAt(node_->GetWorldPosition());
    Light* highestSpot{ spotNode->CreateComponent<Light>() };
    highestSpot->SetLightType(LIGHT_SPOT);
    highestSpot->SetRange(5.0f);
    highestSpot->SetFov(23.5f);
    highestSpot->SetColor(Color(0.6f, 0.7f, 1.0f));
    highestSpot->SetBrightness(5.0f);
    highestSpot->SetSpecularIntensity(0.23f);

    UI* ui{ GetSubsystem<UI>() };
    highestScoreText_ = ui->GetRoot()->CreateChild<Text>();
    highestScoreText_->SetName("HighestScore");
    highestScoreText_->SetText("0");
    if (GRAPHICS){
        highestScoreText_->SetFont(CACHE->GetResource<Font>("Fonts/skirmishergrad.ttf"), static_cast<float>(GRAPHICS->GetHeight() / 50));
    }
    highestScoreText_->SetColor(Color(0.23f, 0.75f, 1.0f, 1.0f));
    highestScoreText_->SetHorizontalAlignment(HA_CENTER);
    highestScoreText_->SetVerticalAlignment(VA_CENTER);
    highestScoreText_->SetPosition(0, ui->GetRoot()->GetHeight()/4.2f);

    Node* pilotNode{ node_->CreateChild("HighestPilot") };
    highestPilot_ = pilotNode->CreateComponent<Pilot>();
    highestPilot_->Initialize(true);

    if (highestScore_ == 0){
        node_->SetEnabledRecursive(false);
        highestScoreText_->SetColor(Color{0.0f, 0.0f, 0.0f, 0.0f});

    } else {
        podiumNode->SetRotation(Quaternion::IDENTITY);
        podiumNode->Rotate(Quaternion(LucKey::RandomSign() * 60.0f, Vector3::UP));
        node_->SetEnabledRecursive(true);
        highestScoreText_->SetText(String(highestScore_));
        highestScoreText_->SetColor(Color(0.23f, 0.75f, 1.0f, 0.75f));
    }

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(Highest, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(Highest, EnterPlay));
}
void Highest::Update(float timeStep)
{
    node_->SetPosition(HIGHEST_POS + Vector3::UP * MC->Sine(0.1f, -0.23f, 0.0f));
}

void Highest::SetPilot(Pilot* pilot, unsigned score)
{
    if (score <= highestScore_)
        return;

    highestPilot_->Clone(pilot);

    SetScore(score);

    pilot->Save(0, highestScore_);

    if (highestScore_ > 0){
        node_->SetEnabledRecursive(true);
        highestScoreText_->SetColor(Color(0.23f, 0.75f, 1.0f, 0.75f));
    }
}

void Highest::SetScore(unsigned score)
{
    highestScore_ = score;
    highestScoreText_->SetText(String(highestScore_));
}

void Highest::EnterLobby(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    node_->SetEnabledRecursive(highestScore_ != 0);
    highestScoreText_->SetColor(Color(0.23f, 0.75f, 1.0f, 0.75f) * static_cast<float>(highestScore_ != 0));
}
void Highest::EnterPlay(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    node_->SetEnabledRecursive(false);
    highestScoreText_->SetColor(Color(0.13f, 0.666f, 1.0f, 0.23f) * static_cast<float>(highestScore_ != 0));
}
