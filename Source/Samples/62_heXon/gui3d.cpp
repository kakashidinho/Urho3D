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

#include "spawnmaster.h"
#include "effectmaster.h"
#include "line.h"
#include "lobby.h"
#include "panel.h"
#include "ship.h"

#include "gui3d.h"

void GUI3D::RegisterObject(Context* context)
{
    context->RegisterFactory<GUI3D>();

    MC->GetSample("Death");
}

GUI3D::GUI3D(Context* context) : LogicComponent(context),
    colorSet_{},
    score_{},
    toCount_{},

    health_{},
    appleCount_{},
    heartCount_{},

    healthIndicator_{},
    deathSource_{},

    subNode_{},
    scoreNode_{},
    scoreDigits_{},
    healthBarNode_{},
    healthBarModel_{},
    shieldBarNode_{},
    shieldBarModel_{},
    appleCounterRoot_{},
    appleCounter_{},
    heartCounterRoot_{},
    heartCounter_{},
    barrels_{},
    barrelCount_{1},
    cannonNode_{nullptr}
{
}

void GUI3D::OnNodeSet(Node* node)
{ if (!node) return;

    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(GUI3D, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(GUI3D, EnterPlay));
}
void GUI3D::Initialize(int colorSet)
{
    colorSet_ = colorSet;

    subNode_ =  node_->CreateChild("Sub");
    subNode_->SetPosition(LOBBYPOS);

  /*  if (MC->GetAspectRatio() < 1.6f) {

        subNode_->Translate(Vector3( (colorSet_ / 2 == 1) ? -0.9f : 0.9f,
                                    0.0f, 0.0f));
    }*/

    float angle{};
    switch(colorSet_){
    case 1: angle =  -60.0f; break;
    case 2: angle =   60.0f; break;
    case 3: angle = -120.0f; break;
    case 4: angle =  120.0f; break;
    }

//    subNode_->Rotate(Quaternion(180.0f * (colorSet_ - 1 % 2), Vector3::FORWARD));
    node_->Rotate(Quaternion(angle, Vector3::UP));

    scoreNode_ = subNode_->CreateChild("Score");
    if (colorSet_ == 4)
        scoreNode_->Rotate(Quaternion(180.0f, 0.0f, 180.0f));

    for (int d{ 0 }; d < 10; ++d) {

        scoreDigits_[d] = scoreNode_->CreateChild("Digit");
        scoreDigits_[d]->SetEnabled( d == 0 );
        scoreDigits_[d]->Translate(Vector3::RIGHT * (colorSet == 2 ? -0.5f : 0.5f) * (d - 4.5f));
        scoreDigits_[d]->Rotate(Quaternion(colorSet == 2 ? 0.0f : 180.0f, Vector3::UP), TS_WORLD);
        scoreDigits_[d]->Rotate(Quaternion((colorSet % 2) * 90.0f - 45.0f, Vector3::RIGHT), TS_LOCAL);
        scoreDigits_[d]->SetScale(0.9f);
        StaticModel* digitModel{ scoreDigits_[d]->CreateComponent<StaticModel>() };
        digitModel->SetModel(MC->GetModel("0"));
        digitModel->SetMaterial(MC->colorSets_[colorSet_].glowMaterial_);
//        digitModel->SetLightMask(0);
    }
//    scoreNode_->SetPosition(Vector3(playerId == 2 ? 5.94252f : -5.94252f, 0.9f, 0.82951f));
//    scoreNode_->Rotate(Quaternion(-90.0f, Vector3::RIGHT));
//    scoreNode_->Rotate(Quaternion(playerId == 2 ? 60.0f : -60.0f, Vector3::UP), TS_WORLD);

    /*Model* barModel = (playerId == 2)
            ? MC->GetModel("BarRight")
            : MC->GetModel("BarLeft");*/

    healthIndicator_ = subNode_->CreateComponent<AnimatedModel>();
    healthIndicator_->SetModel(MC->GetModel("HealthIndicator"));
    healthIndicator_->SetMaterial(0, MC->GetMaterial("GreenGlow")->Clone());
    healthIndicator_->SetMaterial(1, MC->GetMaterial("BlueGlow"));
    healthIndicator_->SetMaterial(2, MC->GetMaterial("Black"));
    healthIndicator_->SetMorphWeight(0, 1.0f);
    healthIndicator_->SetMorphWeight(1, 0.0f);

    appleCounterRoot_ = subNode_->CreateChild("AppleCounter");
//    appleCounterRoot_->Rotate(Quaternion(180.0f * ((colorSet_ - 1) % 2), Vector3::FORWARD));

    for (int a{ 0 }; a < 4; ++a) {

        appleCounter_[a] = appleCounterRoot_->CreateChild();
        appleCounter_[a]->SetEnabled(false);

        if ((colorSet_ - 1) % 2)
            appleCounter_[a]->SetPosition(Vector3::FORWARD * 2.3f + Vector3::RIGHT * (a * -0.666f + 1.0f));
        else
            appleCounter_[a]->SetPosition(Vector3::FORWARD * 2.3f + Vector3::RIGHT * (a * 0.666f - 1.0f));

        appleCounter_[a]->SetScale(0.23f);
        appleCounter_[a]->Rotate(Quaternion(13.0f, Vector3::RIGHT), TS_WORLD);
        StaticModel* apple{ appleCounter_[a]->CreateComponent<StaticModel>() };
        apple->SetModel(MC->GetModel("Apple"));
        apple->SetMaterial(MC->GetMaterial("GoldEnvmap"));
    }

    heartCounterRoot_ = subNode_->CreateChild("HeartCounter");
//    heartCounterRoot_->Rotate(Quaternion(180.0f * ((colorSet_ - 1) % 2), Vector3::FORWARD));

    for (int h{ 0 }; h < 4; ++h) {

        heartCounter_[h] = heartCounterRoot_->CreateChild();
        heartCounter_[h]->SetEnabled(false);

        if ((colorSet_ - 1) % 2)
            heartCounter_[h]->SetPosition(Vector3::FORWARD * 2.3f + Vector3::RIGHT *  (h * -0.666f + 1.0f));
        else
            heartCounter_[h]->SetPosition(Vector3::FORWARD * 2.3f + Vector3::RIGHT * (h * 0.666f - 1.0f));

        heartCounter_[h]->SetScale(0.23f);
        heartCounter_[h]->Rotate(Quaternion(13.0f, Vector3::RIGHT), TS_WORLD);
        StaticModel* heart{ heartCounter_[h]->CreateComponent<StaticModel>() };
        heart->SetModel(MC->GetModel("Heart"));
        heart->SetMaterial(MC->GetMaterial("RedEnvmap"));
    }

    cannonNode_ = subNode_->CreateChild("Cannon");
    cannonNode_->SetPosition(Vector3(2.9f * ((colorSet_ & 2) - 1), -0.13f, 1.55f));
    float parentZ{ cannonNode_->GetParent()->GetWorldPosition().z_ };
    cannonNode_->LookAt(Vector3::BACK * parentZ * 2.7f + Vector3::DOWN * 0.55f, Vector3::BACK * parentZ * 1.0f + Vector3::UP * 8.0f);
//    cannonNode_->SetRotation(Quaternion(-90.0f, Vector3::UP));
    StaticModel* cannonBase{ cannonNode_->CreateComponent<StaticModel>() };
    cannonBase->SetModel(MC->GetModel("CannonBase"));
    cannonBase->SetMaterial(0, MC->colorSets_[colorSet].hullMaterial_);
    cannonBase->SetMaterial(1, MC->colorSets_[colorSet].glowMaterial_);

    for (int c{0}; c < 5; ++c) {

        Node* barrelNode{ cannonNode_->CreateChild("Barrel") };

        float z{ -0.035202f };
        float x{ 0.0f };
        bool front{ c < 3 };
        bool left{ ((c % 2) == 1) ? true:false};
        if (c != 0) {

            float rot{ front ? 17.0f : -17.0f };
            x = front ? 0.136274 : 0.098474;
            if (left) {
                x = -x;
                rot = - rot;
            }
            if (!front)
                rot += 180.0f;
            z = front ? -0.055623f : -0.049149f;

            barrelNode->Rotate(Quaternion(rot, Vector3::UP));
        }
        barrelNode->SetPosition(Vector3(x, 0.0f, z));
        AnimatedModel* barrel{ barrelNode->CreateComponent<AnimatedModel>() };
        barrel->SetModel(MC->GetModel("CannonBarrel"));
        barrel->SetMaterial(0, MC->colorSets_[colorSet_].hullMaterial_);
        barrel->SetMaterial(1, MC->GetMaterial("Metal"));
        barrel->SetMaterial(2, MC->GetMaterial("Black"));
        barrels_.Push(barrel);

        if (c == 0) {
            for (unsigned m{0}; m < barrel->GetNumMorphs(); ++m) {

                barrel->SetMorphWeight(m, 1.0f);
            }
        }
    }

    MC->scene_->CreateChild("Panel")->CreateComponent<Panel>()->Initialize(colorSet_);

    deathSource_ = node_->CreateComponent<SoundSource>();
    deathSource_->SetSoundType(SOUND_EFFECT);
    deathSource_->SetGain(2.3f);
}

void GUI3D::Update(float timeStep)
{
//    SetBarrels(((static_cast<int>(TIME->GetElapsedTime()) / 2) % 5) + 1);

    CountScore();

    for (int i{0}; i < 4; ++i) {

        appleCounter_[i]->Rotate(Quaternion(0.0f, (i * i + 10.0f) * 23.0f * timeStep, 0.0f));
        appleCounter_[i]->SetScale(MC->Sine((0.23f + (appleCount_)) * 0.23f, 0.1f, 0.17f, -i * 2.3f));
        heartCounter_[i]->Rotate(Quaternion(0.0f, (i * i + 10.0f) * 23.0f * timeStep, 0.0f));
        heartCounter_[i]->SetScale(MC->Sine((0.23f + (heartCount_)) * 0.23f, 0.1f, 0.17f, -i * 2.3f));
    }

    //Update HealthBar color
    healthIndicator_->GetMaterial(0)->SetShaderParameter("MatDiffColor", HealthToColor(health_));
    healthIndicator_->GetMaterial(0)->SetShaderParameter("MatEmissiveColor", HealthToColor(health_) * 0.42f);
    healthIndicator_->GetMaterial(0)->SetShaderParameter("MatSpecularColor", HealthToColor(health_) * 0.05f);

    for (unsigned b{0}; b < barrels_.Size(); ++b) {

        AnimatedModel* barrel{ barrels_[b] };
        float weight{};
        if (b == 0) {
            if (!(barrelCount_ & 2))
                weight = 1.0f;
        } else if (b < 3) {
            if (barrelCount_ > 1)
                weight = 1.0f;
        } else {

            if (barrelCount_ > 2)
                weight = 1.0f;
        }
        for (unsigned m{0}; m < barrel->GetNumMorphs(); ++m) {

            if (weight == 1.0f) {
                barrel->SetMorphWeight(m, Lerp(barrel->GetMorphWeight(m), m == 0 ? weight : (barrel->GetMorphWeight(m - 1) > .99f), Min(1.0f, timeStep * 17.0f)));
            } else {
                barrel->SetMorphWeight(m, Lerp(barrel->GetMorphWeight(m), (m == barrel->GetNumMorphs() - 1) ? weight : (barrel->GetMorphWeight(m + 1) > 0.01f), Min(1.0f, timeStep * 17.0f)));

            }
        }
    }
}

void GUI3D::PlayDeathSound()
{
    deathSource_->Play(MC->GetSample("Death"));
}

void GUI3D::SetHealth(float health)
{
    health_ = health;

    healthIndicator_->SetMorphWeight(0, Min(health_, 10.0f) * 0.1f);
    healthIndicator_->SetMorphWeight(1, Max(health_ - 10.0f,  0.0f) * 0.2f);

//    healthBarNode_->SetScale(Vector3(Min(health_, 10.0f), 1.0f, 1.0f));
//    shieldBarNode_->SetScale(Vector3(health_, 0.95f, 0.95f));
}
void GUI3D::SetHeartsAndApples(int hearts, int apples)
{
    appleCount_ = apples;
    heartCount_ = hearts;

    for (int a{0}; a < 4; ++a) {

        appleCounter_[a]->SetEnabled(appleCount_ > a);
    }
    for (int h{0}; h < 4; ++h) {

        heartCounter_[h]->SetEnabled(heartCount_ > h);
    }
}
void GUI3D::SetScore(unsigned score)
{
    if (score > score_)
        toCount_ += score - score_;

    score_ = score;
    //Update score graphics
    for (int d{0}; d < 10; ++d) {
        StaticModel* digitModel{ scoreDigits_[d]->GetComponent<StaticModel>() };
        digitModel->SetModel(MC->GetModel(String(
                             static_cast<int>(score_ / static_cast<unsigned>(pow(10, d))) % 10 )));

        scoreDigits_[d]->SetEnabled( score_ >= static_cast<unsigned>(pow(10, d))
                                       || d == 0 );
    }
}
void GUI3D::SetBarrels(int num)
{
    barrelCount_ = num;
}
void GUI3D::CountScore()
{
    Player* player{ MC->GetPlayerByColorSet(colorSet_) };
    if (!player || !player->IsAlive())
        return;

    int maxLines{ 512 };
    int threshold{ maxLines / (Max(GetSubsystem<SpawnMaster>()->CountActive<Ship>(), 1) * 8) };

    int lines{ GetSubsystem<SpawnMaster>()->CountActive<Line>() };
    int counted{};

    while ((toCount_ > 0)
        && (lines < maxLines)
        && (counted < threshold)) {

        GetSubsystem<SpawnMaster>()->Create<Line>()->Set(colorSet_);
        --toCount_;
        ++lines;
        ++counted;
    }
}

void GUI3D::EnterLobby(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;


    node_->SetEnabledRecursive(true);
    SetHeartsAndApples(0, 0);
    SetScore(score_);

    health_ = 0.0f;
    healthIndicator_->SetMorphWeight(0, 1.0f);
    healthIndicator_->SetMorphWeight(1, 0.0f);

    node_->SetPosition(Vector3::UP);
    node_->SetScale(MC->AspectRatio() > 1.6f ? 1.0f
                                             : 0.85f);
    subNode_->SetScale(0.75f);

    toCount_ = 0;
    cannonNode_->SetEnabled(false);
}
void GUI3D::EnterPlay(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    if (!MC->GetPlayerByColorSet(colorSet_)){
        node_->SetEnabledRecursive(false);
        return;
    }
    SetHeartsAndApples(0, 0);
    SetScore(score_);

    node_->SetPosition(Vector3::DOWN * 1.23f);
    node_->SetScale(3.6f);

    subNode_->SetScale(MC->AspectRatio() > 1.6f ? 1.0f
                                                : 0.75f);

    cannonNode_->SetEnabled(true);
}

Color GUI3D::HealthToColor(float health)
{
    Color color(0.23f, 1.0f, 0.05f, 1.0f);
    health = Clamp(health, 0.0f, 10.0f);
    float maxBright( health < 5.0f ? MC->Sine(0.75f + 0.24f * (4.0f - health), 0.25f * health, 0.666f)
                                   : 0.42f);

    color.r_ = Clamp((3.0f - (health - 3.0f)) / 2.3f, 0.0f, maxBright);
    color.g_ = Clamp((health - 3.0f) / 4.0f, 0.0f, maxBright);
    return color;
}
