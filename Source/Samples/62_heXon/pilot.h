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

#ifndef PILOT_H
#define PILOT_H

#include <Urho3D/Urho3D.h>

#include "controllable.h"

class Ship;

enum PilotColor { PC_SKIN, PC_SHIRT, PC_PANTS, PC_SHOES, PC_HAIR, PC_ALL };
enum Hair{HAIR_BALD, HAIR_SHORT, HAIR_MOHAWK, HAIR_SEAGULL, HAIR_MUSTAIN, HAIR_FROTOAD, HAIR_FLATTOP, HAIR_ALL, HAIR_SANTAHAT, HAIR_PUMPKIN, HAIR_AXE};

class Pilot : public Controllable
{
#define SPAWNPOS Vector3(playerId_ * 0.666f - 1.8f - Random(0.03f), 0.0f, 8.0f)

    URHO3D_OBJECT(Pilot, Controllable);
    friend class Highest;
    friend class Player;
    friend class MasterControl;
public:
    Pilot(Context* context);
    Ship* pickedShip_;

    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void FixedUpdate(float timeStep) override;

    void ClearControl() override;
    void Think() override;

    void Randomize();
    void Initialize(bool highest = false);
    int GetPlayerId() { return playerId_; }
    void Upload();
    void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
    void EnterLobbyFromShip();
    void LeaveLobby();
    void Clone(Pilot *pilot);
    
private:
    int playerId_;
    bool male_;
    bool alive_;
    float deceased_;
    bool autoPilot_;
    int hairStyle_;
    HashMap<int, Color> pilotColors_;
    AnimatedModel* hairModel_;

    void HandleSetControlled();
    void Load();
    void ApplyMovement(float timeStep);
    void UpdateModel();
    void Save(int playerID, unsigned score);
    void Die();
    void Revive();
    void EnterLobbyThroughDoor();
    bool ShipPicked(Ship *ship);
};

#endif // PILOT_H
