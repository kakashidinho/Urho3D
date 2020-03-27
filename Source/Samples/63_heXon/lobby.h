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

#ifndef LOBBY_H
#define LOBBY_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"
#include "mastercontrol.h"

class Highest;

class Lobby : public LogicComponent
{
    URHO3D_OBJECT(Lobby, LogicComponent);
public:
    Lobby(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void EnterLobby(StringHash eventType, VariantMap &eventData);
    void EnterPlay(StringHash eventType, VariantMap &eventData);

    void AddButtonPressed(StringHash eventType, VariantMap &eventData);
    void RemoveButtonPressed(StringHash eventType, VariantMap &eventData);
private:
    Highest* highest_;
};

#endif // LOBBY_H
