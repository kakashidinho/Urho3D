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

#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class GUI3D;
class Ship;


class Player : public Object
{
    URHO3D_OBJECT(Player, Object);
public:
    Player(int playerId, Context* context);
    static HashMap< int, int > takenColorSets_; //Player ID - ColorSet

    Vector3 GetPosition();
    Ship* GetShip();

    int GetPlayerId() const { return playerId_; }
    void AddScore(int points);
    unsigned GetScore() const { return score_; }
    void SetScore(int points);
    unsigned GetFlightScore() const { return flightScore_; }
    void Die();
    void Respawn();
    void ResetScore();

    bool IsAlive() const noexcept { return alive_; }
    bool IsHuman() const noexcept { return !autoPilot_; }
    void EnterLobby(StringHash eventType, VariantMap &eventData);
    void EnterLobby();
    void EnterPlay(StringHash eventType, VariantMap &eventData);

    GUI3D* gui3d_;
private:

    int playerId_;
    bool autoPilot_;
    bool alive_;

    unsigned score_;
    unsigned flightScore_;
    int multiplier_;


};

#endif // PLAYER_H
