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

#include <fstream>

#include "effectmaster.h"
#include "inputmaster.h"
#include "player.h"
#include "ship.h"
#include "door.h"
#include "splatterpillar.h"
#include "highest.h"

#include "pilot.h"

void Pilot::RegisterObject(Context *context)
{
    context->RegisterFactory<Pilot>();
}

Pilot::Pilot(Context* context) : Controllable(context),
    pickedShip_{ nullptr },
    playerId_{0},
    male_{ false },
    alive_{ true },
    deceased_{ 0.0f },
    autoPilot_{ false },
    hairStyle_{ 0 },
    pilotColors_{}
{
    thrust_ = 1024.0f;
    maxSpeed_ = 1.23f;
}

void Pilot::OnNodeSet(Node *node)
{ if (!node) return;

    Controllable::OnNodeSet(node);

    model_->SetModel(MC->GetModel("Male"));
    model_->SetCastShadows(true);
    Node* head{ node_->GetChild("Head", true) };
    Node* hairNode{ head->CreateChild("Hair") };
    hairModel_ = hairNode->CreateComponent<AnimatedModel>();
    hairModel_->SetCastShadows(true);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetLinearDamping(0.88f);
    rigidBody_->SetLinearRestThreshold(0.0f);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    rigidBody_->SetAngularRestThreshold(0.0f);
    collisionShape_->SetCapsule(0.42f, 0.5f, Vector3::UP * 0.3f);

    //Also animates highest
    animCtrl_->PlayExclusive("Models/IdleAlert.ani", 0, true);
    animCtrl_->SetSpeed("Models/IdleAlert.ani", 0.5f);
    animCtrl_->SetStartBone("Models/IdleAlert.ani", "MasterBone");

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Pilot, HandleNodeCollisionStart));
}

void Pilot::ApplyMovement(float timeStep)
{
    bool run{ actions_[0] };
    Vector3 force{ move_.Length() < 0.05f ? Vector3::ZERO : move_ * thrust_ * timeStep };
    force *= 1.0f + 0.23f * run;

    if ( rigidBody_->GetLinearVelocity().Length() < (maxSpeed_ * (1.0f + 0.42f * run))
     || (rigidBody_->GetLinearVelocity().Normalized() + force.Normalized()).Length() < M_SQRT2 )
    {
        rigidBody_->ApplyForce(force);
    }
}
void Pilot::FixedUpdate(float timeStep)
{
    ApplyMovement(timeStep);
}

void Pilot::Update(float timeStep)
{
    if (node_->GetName() == "HighestPilot")
        return;

    if (!alive_){
        deceased_ += timeStep;
        if (deceased_ > 0.125f)
            Revive();

        return;
    }

    Controllable::Update(timeStep);

    //Update rotation according to direction of the player's movement.
    Vector3 velocity{ rigidBody_->GetLinearVelocity() };
//    Vector3 lookDirection{ velocity + 2.0f * aim_ };
//    Quaternion rotation{ node_->GetWorldRotation() };
//    Quaternion aimRotation{ rotation };
//    aimRotation.FromLookRotation(lookDirection);
//    node_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));
    AlignWithMovement(timeStep);

    //Update animation
    if (velocity.Length() > 0.1f){
        animCtrl_->PlayExclusive("Models/WalkRelax.ani", 1, true, 0.15f);
        animCtrl_->SetSpeed("Models/WalkRelax.ani", velocity.Length() * 2.3f);
        animCtrl_->SetStartBone("Models/WalkRelax.ani", "MasterBone");
    }
    else {
        animCtrl_->PlayExclusive("Models/IdleRelax.ani", 1, true, 0.15f);
        animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");
    }
}

void Pilot::Initialize(bool highest)
{
    pickedShip_ = nullptr;

    if (highest) {
        rigidBody_->SetKinematic(true);
        Load();
    } else {
        Randomize();
    }
}

void Pilot::Save(int playerID, unsigned score)
{
    using namespace std;
    ofstream fPilot{};
    fPilot.open((MC->GetResourceFolder() + "/.Pilot" + String(playerID) + ".lkp").CString());
    fPilot << male_ << '\n';
    fPilot << hairStyle_ << '\n';
    for (Color c : pilotColors_.Values()) {
        fPilot << c.r_ << ' '
               << c.g_ << ' '
               << c.b_ << ' '
               << '\n';
    }
    fPilot << score;
}

void Pilot::Load()
{
    using namespace std;

    unsigned score{};

    ifstream fPilot{ (MC->GetResourceFolder() + "/.Pilot" + String(playerId_) + ".lkp").CString() };
    while (!fPilot.eof()){
        string gender_str{};
        string hairStyle_str{};
        string color1_r_str{}, color1_g_str{}, color1_b_str{};
        string color2_r_str{}, color2_g_str{}, color2_b_str{};
        string color3_r_str{}, color3_g_str{}, color3_b_str{};
        string color4_r_str{}, color4_g_str{}, color4_b_str{};
        string color5_r_str{}, color5_g_str{}, color5_b_str{};
        string score_str;

        fPilot >> gender_str;
        if (gender_str.empty()) break;
        fPilot >>
                hairStyle_str >>
                color1_r_str >> color1_g_str >> color1_b_str >>
                color2_r_str >> color2_g_str >> color2_b_str >>
                color3_r_str >> color3_g_str >> color3_b_str >>
                color4_r_str >> color4_g_str >> color4_b_str >>
                color5_r_str >> color5_g_str >> color5_b_str >>
                score_str;

        male_ = static_cast<bool>(stoi(gender_str));
        hairStyle_ = stoi(hairStyle_str);
        pilotColors_.Clear();
        pilotColors_[PC_SKIN]   = Color(stof(color1_r_str), stof(color1_g_str), stof(color1_b_str));
        pilotColors_[PC_SHIRT]  = Color(stof(color2_r_str), stof(color2_g_str), stof(color2_b_str));
        pilotColors_[PC_PANTS]  = Color(stof(color3_r_str), stof(color3_g_str), stof(color3_b_str));
        pilotColors_[PC_SHOES]  = Color(stof(color4_r_str), stof(color4_g_str), stof(color4_b_str));
        pilotColors_[PC_HAIR]   = Color(stof(color5_r_str), stof(color5_g_str), stof(color5_b_str));

        score = static_cast<unsigned>(stoul(score_str, 0, 10));
    }

    if (!pilotColors_.Size() || score == 0)
        Randomize();

//    if (GetPlayer())
//        GetPlayer()->SetScore(score);

    else if (playerId_ == 0) {
        for (Highest* highest : MC->GetComponentsInScene<Highest>()) {

            highest->SetScore(score);
        }
    }

    UpdateModel();

    if (node_->GetName() != "HighestPilot")
        EnterLobbyThroughDoor();
}

void Pilot::HandleSetControlled()
{
    playerId_ = GetPlayer()->GetPlayerId();
}

void Pilot::UpdateModel()
{
    maxSpeed_ = 1.23f + 0.5f * pilotColors_[static_cast<int>(PC_SHOES)].r_;

    //Set body model
    if (male_)  model_->SetModel(MC->GetModel("Male"));
    else        model_->SetModel(MC->GetModel("Female"));

    model_->SetMorphWeight(0, Random());

    //Set colors for body model
    for (unsigned c{PC_SKIN}; c < PC_ALL; ++c){

        model_->SetMaterial(c, MC->GetMaterial("Basic")->Clone());
        Color diffColor{ pilotColors_[c] };
        if (c == 4){

            if (hairStyle_ == HAIR_BALD || hairStyle_ == HAIR_SANTAHAT)
                diffColor = pilotColors_[0];
            else if (hairStyle_ == HAIR_MOHAWK)
                diffColor = LucKey::RandomHairColor(true);
        }
        model_->GetMaterial(c)->SetShaderParameter("MatDiffColor", diffColor);
        Color specColor{ diffColor * (1.0f - 0.1f * c) };
        specColor.a_ = 23.0f - 4.0f * c;
        model_->GetMaterial(c)->SetShaderParameter("MatSpecColor", specColor);
    }

    //Set hair model
    hairModel_->GetNode()->SetScale(1.0f - (0.1f * !male_));

    switch (hairStyle_){
    default: case HAIR_BALD: case HAIR_SHORT:
        hairModel_->SetModel(nullptr);
        break;
    case HAIR_MOHAWK:
        hairModel_->SetModel(MC->GetModel("Mohawk"));
        break;
    case HAIR_SEAGULL:
        hairModel_->SetModel(MC->GetModel("Seagull"));
        break;
    case HAIR_MUSTAIN:
        hairModel_->SetModel(MC->GetModel("Mustain"));
        break;
    case HAIR_FROTOAD:
        hairModel_->SetModel(MC->GetModel("Frotoad"));
        break;
    case HAIR_FLATTOP:
        hairModel_->SetModel(MC->GetModel("Flattop"));
        break;
    case HAIR_SANTAHAT:
        hairModel_->SetModel(MC->GetModel("SantaHat"));
        break;
    case HAIR_PUMPKIN:
        hairModel_->SetModel(MC->GetModel("Pumpkin"));
        break;
    case HAIR_AXE:
        hairModel_->SetModel(MC->GetModel("Axe"));
        break;
    }
    //Set hair color
    if (hairStyle_ == HAIR_SANTAHAT || hairStyle_ == HAIR_PUMPKIN || hairStyle_ == HAIR_AXE) {

        hairModel_->SetMaterial(MC->GetMaterial("VCol"));

    } else if (hairStyle_ != HAIR_BALD && hairStyle_ != HAIR_SHORT)
    {
        hairModel_->SetMorphWeight(0, Random());

        //Set color for hair model
        hairModel_->SetMaterial(MC->GetMaterial("Basic")->Clone());

        Color diffColor{ pilotColors_[4] };
        hairModel_->GetMaterial()->SetShaderParameter("MatDiffColor", diffColor);

        Color specColor{ diffColor * 0.23f };
        specColor.a_ = 23.0f;
        hairModel_->GetMaterial()->SetShaderParameter("MatSpecColor", specColor);
    }
}

void Pilot::Randomize()
{
    male_ = Random(2);

    bool isHuman{ GetPlayer() && GetPlayer()->IsHuman() };
    String timeStamp{ TIME->GetTimeStamp() };

    if (timeStamp.Contains("Oct 2")
     || timeStamp.Contains("Oct 3"))

        hairStyle_ = isHuman ? HAIR_PUMPKIN : HAIR_AXE;

    else if (isHuman && timeStamp.Contains("Dec"))

        hairStyle_ = HAIR_SANTAHAT;
    else
        hairStyle_ = Random(static_cast<int>(HAIR_ALL));

    for (int c{PC_SKIN}; c < PC_ALL; ++c) {
        switch (c){
        case 0:
            pilotColors_[c] = (autoPilot_
                               ? Color::GRAY * 0.666f
                               : LucKey::RandomSkinColor());
            break;
        case 4:
            pilotColors_[c] = LucKey::RandomHairColor();
            break;
        default: pilotColors_[c] = LucKey::RandomColor();
            break;
        }
    }
    UpdateModel();
}

void Pilot::Upload()
{
    GetSubsystem<EffectMaster>()->TranslateTo(node_, node_->GetPosition() + 2.0f * Vector3::UP, 0.1f);
    GetSubsystem<EffectMaster>()->ScaleTo(node_, Vector3::ONE - Vector3::UP * 0.9f, 0.125f);
    Die();
    animCtrl_->SetSpeed("Models/WalkRelax.ani", 0.0f);
    animCtrl_->SetSpeed("Models/IdleRelax.ani", 0.0f);
    rigidBody_->SetKinematic(true);

    for (Highest* highest : MC->GetComponentsInScene<Highest>()) {

        highest->SetPilot(this, GetPlayer()->GetScore());
    }
}

void Pilot::Clone(Pilot* pilot)
{

    male_ = pilot->male_;
    hairStyle_ = pilot->hairStyle_;
    pilotColors_ = pilot->pilotColors_;

    UpdateModel();
}

void Pilot::Die()
{
    alive_ = false;
    path_.Clear();
    pickedShip_ = nullptr;
}

void Pilot::EnterLobbyThroughDoor()
{
    node_->SetEnabledRecursive(true);

    node_->SetPosition(SPAWNPOS);
    path_.Clear();
    path_.Push(Vector3(node_->GetPosition().x_, node_->GetPosition().y_, 4.5f));

    node_->SetRotation(Quaternion(180.0f, Vector3::UP));
    rigidBody_->ApplyImpulse(Vector3::BACK * 3.4f);


    MC->GetComponentsInScene<Door>()[0]->hasBeenOpen_ = false;
}
void Pilot::EnterLobbyFromShip()
{
    node_->SetEnabledRecursive(true);

    for (Ship* s : MC->GetComponentsInScene<Ship>()) {

        if (s->GetColorSet() == Player::takenColorSets_[playerId_]) {

            Set(s->GetPosition() * Vector3(1.7f, 0.0f, 1.7f),
                s->GetNode()->GetRotation());
            rigidBody_->ApplyImpulse(node_->GetDirection());
        }
    }
}

void Pilot::Revive()
{
    Randomize();
    alive_ = true;
    GetPlayer()->Respawn();

    node_->SetAttributeAnimation("Position", nullptr);
    node_->SetAttributeAnimation("Scale", nullptr);
    rigidBody_->SetKinematic(false);
    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    rigidBody_->ResetForces();

    node_->SetScale(Vector3::ONE);
    animCtrl_->SetSpeed("Models/IdleRelax.ani", 1.0f);

    if (GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_) != this)
        GetSubsystem<InputMaster>()->SetPlayerControl(playerId_, this);

    EnterLobbyThroughDoor();
}

void Pilot::ClearControl()
{
    Controllable::ClearControl();

    node_->SetEnabledRecursive(false);
}

void Pilot::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

//    if (!static_cast<RigidBody*>(eventData[NodeCollisionStart::P_OTHERBODY].GetPtr())->IsTrigger())
//        path_.Clear();

    Node* otherNode{ static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr()) };

    Ship* ship{ otherNode->GetComponent<Ship>() };
    if (ship) {

        for (int p : Player::takenColorSets_.Keys()){
            //Don't get in if the pilot has a ship and this is not the pilot's ship
            if (p == playerId_) {

                if ( Player::takenColorSets_[p] != ship->GetColorSet()) {
                    return;
                }

            //Don't get in if the ship is taken by another player
            } else if (Player::takenColorSets_[p] == ship->GetColorSet()) {
                return;
            }
        }

        GetSubsystem<InputMaster>()->SetPlayerControl(playerId_, ship);
        //Indicate ready
        ship->SetHealth(10.0f);
    }
}

void Pilot::Think()
{
    Controllable::Think();

    //Only walk if there are pathnodes left
    if (path_.Size() || GetPlayer()->IsHuman())
        return;

    SplatterPillar* splatterPillar{};
    for (SplatterPillar* s : MC->GetComponentsInScene<SplatterPillar>())
        splatterPillar = s;

    bool splatterPillarIdle{ splatterPillar->IsIdle() };

    if (Player::takenColorSets_.Contains(GetPlayerId())) {

        pickedShip_ = MC->GetShipByColorSet(Player::takenColorSets_[GetPlayerId()]);

    } else {
        for (Ship* ship : MC->GetComponentsInScene<Ship>()) {

            if (ShipPicked(ship))
                continue;

            if (!Player::takenColorSets_.Values().Contains(ship->GetColorSet())) {

                pickedShip_ = ship;
                break;
            }
        }
    }

    //Pick a destination

    //Enter play
    if ( pickedShip_ && ((MC->AllPlayersScoreZero(false) && MC->NoHumans())
                     || (MC->AllReady(true) && !MC->NoHumans()))) {

        NAVMESH->FindPath(path_, node_->GetPosition(), pickedShip_->GetPosition() + pickedShip_->GetNode()->GetDirection() * 0.23f);
        path_.Push(pickedShip_->GetPosition());

    //Reset Score
    } else if (GetPlayer()->GetScore() != 0 && (MC->NoHumans() || MC->AllPlayersScoreZero(true))
            && splatterPillarIdle) {

        NAVMESH->FindPath(path_, node_->GetPosition(), splatterPillar->GetPosition());

    //Exit
    } else if (!MC->NoHumans() && MC->GetDoor()->HidesAllPilots(true)) {

        LeaveLobby();

    } else if (node_->GetPosition().z_ > 6.0f){
        MC->RemovePlayer(GetPlayer());
    }
    //Stay put
    else
        SetMove(Vector3::ZERO);

    SetAim(Vector3::ZERO);
}

void Pilot::LeaveLobby()
{
    NAVMESH->FindPath(path_, node_->GetPosition(), SPAWNPOS);
}

bool Pilot::ShipPicked(Ship* ship)
{
    /*for (int p : Player::colorSets_.Keys()) {
        int colorSet{};
        if (p != GetPlayerId() && Player::colorSets_.TryGetValue(p, colorSet) && colorSet == ship->GetColorSet())

            return true;
    }*/

    for (Pilot* p : MC->GetComponentsInScene<Pilot>())
        if (p != this && p->pickedShip_ == ship)
            return true;

    return false;
}
