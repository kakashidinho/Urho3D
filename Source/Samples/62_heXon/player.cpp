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

#include "player.h"

#include "TailGenerator.h"

#include "mastercontrol.h"
#include "hexocam.h"
#include "inputmaster.h"
#include "spawnmaster.h"
#include "arena.h"
#include "bullet.h"
#include "muzzle.h"
#include "chaoflash.h"
#include "explosion.h"
#include "heart.h"
#include "apple.h"
#include "chaoball.h"
#include "pilot.h"
#include "splatterpillar.h"
#include "door.h"
#include "ship.h"
#include "phaser.h"
#include "gui3d.h"

HashMap<int, int> Player::takenColorSets_{};

Player::Player(int playerId, Context* context): Object(context),
    playerId_{playerId},
    autoPilot_{playerId_ != 1 && !GetSubsystem<Input>()->GetJoystickByIndex(playerId_ - 1)},
//    autoPilot_{false},
//    autoPilot_{true},
    alive_{false},
    score_{0},
    flightScore_{0},
    multiplier_{1},
    gui3d_{nullptr}
{
    SubscribeToEvent(E_ENTERLOBBY, URHO3D_HANDLER(Player, EnterLobby));
    SubscribeToEvent(E_ENTERPLAY,  URHO3D_HANDLER(Player, EnterPlay));
}

void Player::Die()
{
    alive_ = false;
}
void Player::Respawn()
{
    ResetScore();
    takenColorSets_.Erase(playerId_);

    alive_ = true;
}

void Player::SetScore(int points)
{
    score_ = points;
    if (gui3d_ != nullptr)
        gui3d_->SetScore(score_);

}
void Player::ResetScore()
{
    SetScore(0);
}

void Player::EnterLobby(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    EnterLobby();
}
void Player::EnterLobby()
{
    multiplier_ = 1;

    for (Pilot* pilot : MC->GetComponentsInScene<Pilot>()) {
        if (playerId_ == pilot->GetPlayerId()) {
            GetSubsystem<InputMaster>()->SetPlayerControl(playerId_, pilot);

            if (!alive_) {
                pilot->Revive();

            } else {
                pilot->EnterLobbyFromShip();
            }
        }
    }
}

void Player::EnterPlay(StringHash eventType, VariantMap &eventData)
{
    flightScore_ = 0;
}

void Player::AddScore(int points)
{
    if (!alive_ || points == 0)
        return;

    points *= static_cast<int>(pow(2.0, static_cast<double>(multiplier_ - 1)));
    SetScore(GetScore() + points);
    //Check for multiplier increase
    for (int i{0}; i < 10; ++i){
        unsigned tenPow{static_cast<unsigned>(pow(10, i))};
        if (flightScore_ < tenPow && (flightScore_ + points) > tenPow){
            ++multiplier_;
            GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->
                    PlaySample(MC->GetSample("MultiX"), 0.42f);
            Ship* ship{ GetShip() };
            if (ship)
                MC->arena_->FlashX(MC->colorSets_[ship->GetColorSet()].colors_.first_);
            break;
        }
    }
    flightScore_ += points;
}

Vector3 Player::GetPosition()
{
    return GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->GetPosition();
}

Ship* Player::GetShip()
{
    Controllable* controllable{ GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_) };
    if (controllable->IsInstanceOf<Ship>()) {

        Ship* ship{ static_cast<Ship*>(controllable) };
        return ship;
    }
    return nullptr;
}
