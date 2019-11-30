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

#ifndef GUI3D_H
#define GUI3D_H

#include "mastercontrol.h"

#define LOBBYPOS Vector3::FORWARD * 6.42f

class GUI3D : public LogicComponent
{
    URHO3D_OBJECT(GUI3D, LogicComponent);
public:
    GUI3D(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep) override;
    void OnNodeSet(Node* node) override;

    void Initialize(int colorSet);
    void EnterLobby(StringHash eventType, VariantMap &eventData);
    void EnterPlay(StringHash eventType, VariantMap &eventData);
    void SetHealth(float health);
    void SetHeartsAndApples(int hearts, int apples);
    void SetScore(unsigned score);
    void PlayDeathSound();
    void SetBarrels(int num);
private:
    int colorSet_;
    unsigned score_;
    unsigned toCount_;

    float health_;
    int appleCount_;
    int heartCount_;
    int barrelCount_;
    Node* cannonNode_;

    AnimatedModel* healthIndicator_;
    SoundSource* deathSource_;

    float aspectRatio_;
    Node* subNode_;
    Node* scoreNode_;
    HashMap< int, Node* > scoreDigits_;
    Node* healthBarNode_;
    StaticModel* healthBarModel_;
    Node* shieldBarNode_;
    StaticModel* shieldBarModel_;
    Node* appleCounterRoot_;
    HashMap< int, Node* > appleCounter_;
    Node* heartCounterRoot_;
    HashMap< int, Node* > heartCounter_;
    Vector<AnimatedModel*> barrels_;
    Color HealthToColor(float health);
    void CountScore();
    Vector3 GetOffset();
};

#endif // GUI3D_H
