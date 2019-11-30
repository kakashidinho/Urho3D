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

#ifndef INPUTMASTER_H
#define INPUTMASTER_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "controllable.h"

// Define events to be bound
//URHO3D_EVENT(MENU_BUTTON_DOWN, MenuButtonDown) // MouseDown, KeyDown, ControllerButtonDown, MouseWheel
//{
//    URHO3D_PARAM(P_ACTION, Action);	// int
//}
//URHO3D_EVENT(MENU_BUTTON_UP, MenuButtonUp) // MouseUp, KeyUp, ControllerButtonUp, ControllerAxisMove, MouseWheel
//{
//    URHO3D_PARAM(P_ACTION, Action);	// int
//}
//URHO3D_EVENT(BUTTON_DOWN, ButtonDown) // MouseDown, KeyDown, ControllerButtonDown, MouseWheel
//{
//    URHO3D_PARAM(P_ACTION, Action);	// int
//}
//URHO3D_EVENT(BUTTON_UP, ButtonUp) // MouseUp, KeyUp, ControllerButtonUp, MouseWheel
//{
//    URHO3D_PARAM(P_ACTION, Action);	// int
//}
//URHO3D_EVENT(MOUSE_AXIS_MOVE, MouseAxisMove) // MouseMove
//{
//    URHO3D_PARAM(P_AXIS, Axis);	// int
//    URHO3D_PARAM(P_DELTA, Delta);	// float
//}

enum class MasterInputAction { UP, RIGHT, DOWN, LEFT, CONFIRM, CANCEL, PAUSE, MENU, SCREENSHOT };
enum class PlayerInputAction { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
                               FIRE_N, FIRE_NE, FIRE_E, FIRE_SE,
                               FIRE_S, FIRE_SW, FIRE_W, FIRE_NW,
                               RAM, DIVE, DEPTHCHARGE, REPEL, EJECT };
enum class PlayerInputAxis { MOVE_X, MOVE_Y, FIRE_X, FIRE_Y };

struct InputActions {
    Vector<MasterInputAction> master_;
    HashMap<int, Vector<PlayerInputAction>> player_;
};

class InputMaster : public Object
{
    URHO3D_OBJECT(InputMaster, Object);
public:
    InputMaster(Context* context);
    void SetPlayerControl(int player, Controllable* controllable);
    Player *GetPlayerByControllable(Controllable* controllable);
    Controllable*  GetControllableByPlayer(int playerId);
    Vector<Controllable*>  GetControlled() { return controlledByPlayer_.Values(); }
private:
    HashMap<int, MasterInputAction> keyBindingsMaster_;
    HashMap<int, MasterInputAction> buttonBindingsMaster_;
    HashMap<int, HashMap<int, PlayerInputAction>> keyBindingsPlayer_;
    HashMap<int, HashMap<int, PlayerInputAction>> buttonBindingsPlayer_;
    HashMap<int, HashMap<int, PlayerInputAxis>> axisBindingsPlayer_;

    Vector<int> pressedKeys_;
    HashMap<int, Vector<ControllerButton>> pressedJoystickButtons_;
    HashMap<int, Vector2> leftStickPosition_;
    HashMap<int, Vector2> rightStickPosition_;

    HashMap<int, Controllable*> controlledByPlayer_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData);

    void HandleActions(const InputActions &actions);
    void HandlePlayerAction(PlayerInputAction action, int playerId);
    Vector3 GetMoveFromActions(Vector<PlayerInputAction>* actions);
    Vector3 GetAimFromActions(Vector<PlayerInputAction>* actions);
    void Screenshot();

    void PauseButtonPressed();
    void EjectButtonPressed(int playerID);
};

#endif // INPUTMASTER_H
