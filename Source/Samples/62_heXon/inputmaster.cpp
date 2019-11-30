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

#include "inputmaster.h"

#include "player.h"
#include "ship.h"

InputMaster::InputMaster(Context* context):
    Object(context),
    controlledByPlayer_{}
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_DPAD_UP)]    = MasterInputAction::UP;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_DPAD_DOWN)]  = MasterInputAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_DPAD_LEFT)]  = MasterInputAction::LEFT;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_DPAD_RIGHT)] = MasterInputAction::RIGHT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_A)]          = MasterInputAction::CONFIRM;
    keyBindingsMaster_[KEY_ESCAPE] = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_B)]          = MasterInputAction::CANCEL;
    keyBindingsMaster_[KEY_P]      = buttonBindingsMaster_[static_cast<int>(CONTROLLER_BUTTON_START)]      = MasterInputAction::PAUSE;
    keyBindingsMaster_[KEY_ESCAPE] = MasterInputAction::MENU;

    keyBindingsPlayer_[1][KEY_W] = PlayerInputAction::MOVE_UP;
    keyBindingsPlayer_[1][KEY_S] = PlayerInputAction::MOVE_DOWN;
    keyBindingsPlayer_[1][KEY_A] = PlayerInputAction::MOVE_LEFT;
    keyBindingsPlayer_[1][KEY_D] = PlayerInputAction::MOVE_RIGHT;

    keyBindingsPlayer_[2][KEY_UP]     = PlayerInputAction::MOVE_UP;
    keyBindingsPlayer_[2][KEY_DOWN]   = PlayerInputAction::MOVE_DOWN;
    keyBindingsPlayer_[2][KEY_LEFT]   = PlayerInputAction::MOVE_LEFT;
    keyBindingsPlayer_[2][KEY_RIGHT]  = PlayerInputAction::MOVE_RIGHT;

    keyBindingsPlayer_[1][KEY_KP_8]   = PlayerInputAction::FIRE_N;
    keyBindingsPlayer_[1][KEY_KP_5]   = PlayerInputAction::FIRE_S;
    keyBindingsPlayer_[1][KEY_KP_2]   = PlayerInputAction::FIRE_S;
    keyBindingsPlayer_[1][KEY_KP_4]   = PlayerInputAction::FIRE_W;
    keyBindingsPlayer_[1][KEY_KP_6]   = PlayerInputAction::FIRE_E;
    keyBindingsPlayer_[1][KEY_KP_9]   = PlayerInputAction::FIRE_NE;
    keyBindingsPlayer_[1][KEY_KP_3]   = PlayerInputAction::FIRE_SE;
    keyBindingsPlayer_[1][KEY_KP_1]   = PlayerInputAction::FIRE_SW;
    keyBindingsPlayer_[1][KEY_KP_7]   = PlayerInputAction::FIRE_NW;
    keyBindingsPlayer_[1][KEY_LSHIFT] = PlayerInputAction::RAM;

    for (unsigned p : {1, 2, 3, 4}) {

        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_DPAD_UP]     = PlayerInputAction::MOVE_UP;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_DPAD_DOWN]   = PlayerInputAction::MOVE_DOWN;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_DPAD_LEFT]   = PlayerInputAction::MOVE_LEFT;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_DPAD_RIGHT]  = PlayerInputAction::MOVE_RIGHT;

        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_A]           = PlayerInputAction::RAM;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_B]           = PlayerInputAction::REPEL;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_X]           = PlayerInputAction::DIVE;
        buttonBindingsPlayer_[p][CONTROLLER_BUTTON_Y]           = PlayerInputAction::DEPTHCHARGE;

        axisBindingsPlayer_[p][CONTROLLER_AXIS_LEFTX]           = PlayerInputAxis::MOVE_X;
        axisBindingsPlayer_[p][CONTROLLER_AXIS_LEFTY]           = PlayerInputAxis::MOVE_Y;
        axisBindingsPlayer_[p][CONTROLLER_AXIS_RIGHTX]          = PlayerInputAxis::FIRE_X;
        axisBindingsPlayer_[p][CONTROLLER_AXIS_RIGHTY]          = PlayerInputAxis::FIRE_Y;
    }

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoystickButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    InputActions activeActions{};
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        Vector<PlayerInputAction> emptyActions{};
        activeActions.player_[pId] = emptyActions;
    }

    //Convert key presses to actions
    for (int key : pressedKeys_){
        //Check for master key presses
        if (keyBindingsMaster_.Contains(key)){
            MasterInputAction action{keyBindingsMaster_[key]};
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        //Check for player key presses
        for (Player* p : MC->GetPlayers()){

            int pId{ p->GetPlayerId() };
            if (keyBindingsPlayer_[pId].Contains(key)){
                PlayerInputAction action{keyBindingsPlayer_[pId][key]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
        }
    }
    //Check for joystick button presses
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        Vector<ControllerButton> pressedButtons{ pressedJoystickButtons_[pId - 1] };

        for (int button : pressedButtons)
            if (buttonBindingsPlayer_[pId].Contains(button)){
                PlayerInputAction action{ buttonBindingsPlayer_[pId][button]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
        //Handle eject
        if (pressedButtons.Contains(CONTROLLER_BUTTON_LEFTSHOULDER) && pressedButtons.Contains(CONTROLLER_BUTTON_RIGHTSHOULDER))
            if (p->GetShip())
                p->GetShip()->Eject();
    }

    //Handle the registered actions
    HandleActions(activeActions);
}

void InputMaster::SetPlayerControl(int playerId, Controllable* controllable)
{

    if (controlledByPlayer_.Contains(playerId)){
        if (controlledByPlayer_[playerId] == controllable)
            return;
        else
            controlledByPlayer_[playerId]->ClearControl();
    }

    if (controllable != nullptr){

        controlledByPlayer_[playerId] = controllable;
        controllable->HandleSetControlled();

    } else

        controlledByPlayer_.Erase(playerId);
}

Player* InputMaster::GetPlayerByControllable(Controllable* controllable)
{
    for (int k : controlledByPlayer_.Keys())
    {

        if (controlledByPlayer_[k] == controllable)
            return MC->GetPlayer(k);
    }
    return nullptr;
}
Controllable* InputMaster::GetControllableByPlayer(int playerId)
{
    return controlledByPlayer_[playerId];
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    for (MasterInputAction action : actions.master_){
        switch (action){
        case MasterInputAction::UP:                 break;
        case MasterInputAction::DOWN:               break;
        case MasterInputAction::LEFT:               break;
        case MasterInputAction::RIGHT:              break;
        case MasterInputAction::CONFIRM:            break;
        case MasterInputAction::CANCEL:             break;
        case MasterInputAction::PAUSE:              break;
        case MasterInputAction::MENU:               break;
        default: break;
        }
    }

    //Handle player actions
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        auto playerInputActions = actions.player_[pId];

        Controllable* controlled{ controlledByPlayer_[pId] };
        if (controlled && controlled->GetPlayer()->IsHuman()) {
            if (!controlled->HasPath()) {

                Vector3 stickMove{ Vector3(leftStickPosition_[pId-1].x_, 0.0f, leftStickPosition_[pId-1].y_) };
                Vector3 stickAim{  Vector3(rightStickPosition_[pId-1].x_, 0.0f, rightStickPosition_[pId-1].y_) };

                controlled->SetMove(GetMoveFromActions(playerInputActions) + stickMove);
                controlled->SetAim(GetAimFromActions(playerInputActions) + stickAim);

                ControllableActions restActions{};
                restActions[0] = playerInputActions->Contains(PlayerInputAction::RAM);

                controlled->SetActions(restActions);
            }
        }
    }
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{eventData[KeyDown::P_KEY].GetInt()};

    if (!pressedKeys_.Contains(key))
        pressedKeys_.Push(key);

    switch (key){
    //Exit when ESC is pressed
    case KEY_ESCAPE:
        EjectButtonPressed(0);
        break;
    //Take screenshot when 9 is pressed
    case KEY_9:
    {
        Screenshot();
    } break;
    //Pause/Unpause game on P or joystick Start
    case KEY_P:
    {
        PauseButtonPressed();
    } break;
    //Toggle music on M
    case KEY_M: AUDIO->SetMasterGain(SOUND_MUSIC, AUDIO->GetMasterGain(SOUND_MUSIC) == 0.0f ? 0.32f : 0.0f);
        break;
    }
}
void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{ eventData[KeyUp::P_KEY].GetInt() };

    if (pressedKeys_.Contains(key))
        pressedKeys_.Remove(key);
}

void InputMaster::HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    ControllerButton button{ static_cast<ControllerButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };

    if (!pressedJoystickButtons_[joystickId].Contains(button))
        pressedJoystickButtons_[joystickId].Push(button);

    JoystickState* joystickState{INPUT->GetJoystickByIndex(joystickId)};
    // Process game event
    switch (button) {
    case CONTROLLER_BUTTON_START: PauseButtonPressed();
        break;
    case CONTROLLER_BUTTON_LEFTSHOULDER: case CONTROLLER_BUTTON_RIGHTSHOULDER:
        if (joystickState->GetButtonDown(CONTROLLER_BUTTON_LEFTSHOULDER) &&
                joystickState->GetButtonDown(CONTROLLER_BUTTON_RIGHTSHOULDER))
            EjectButtonPressed(static_cast<int>(joystickId + 1));
        break;
//    case SB_L1: case SB_R1:
//        if (joystickState->GetButtonDown(SB_L1) &&
//                joystickState->GetButtonDown(SB_R1))
//            Screenshot();
//        break;
    default: break;
    }
}
void InputMaster::HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    ControllerButton button{ static_cast<ControllerButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };

    if (pressedJoystickButtons_[joystickId].Contains(button))
        pressedJoystickButtons_[joystickId].Remove(button);
}

void InputMaster::HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickAxisMove::P_JOYSTICKID].GetInt() };
    int axis{ eventData[JoystickAxisMove::P_AXIS].GetInt() };
    float position{ eventData[JoystickAxisMove::P_POSITION].GetFloat() };

    const HashMap<int, PlayerInputAxis>& axisBindings{ axisBindingsPlayer_[joystickId + 1] };
    PlayerInputAxis controlAxis{};
    if (axisBindings.TryGetValue(axis, controlAxis)) {

        if (controlAxis == PlayerInputAxis::MOVE_X) {
            leftStickPosition_[joystickId].x_ =  position;

        } else if (controlAxis == PlayerInputAxis::MOVE_Y) {
            leftStickPosition_[joystickId].y_ = -position;

        } else if (controlAxis == PlayerInputAxis::FIRE_X) {
            rightStickPosition_[joystickId].x_ =  position;

        } else if (controlAxis == PlayerInputAxis::FIRE_Y) {
            rightStickPosition_[joystickId].y_ = -position;

        }
    }
}

void InputMaster::PauseButtonPressed()
{
    switch (MC->GetGameState()) {
    case GS_INTRO: break;
    case GS_LOBBY: /*MC->SetGameState(GS_PLAY);*/ break;
    case GS_PLAY: MC->SetPaused(!MC->IsPaused()); break;
    case GS_DEAD: MC->SetGameState(GS_LOBBY); break;
    case GS_EDIT: break;
        default: break;
    }
}

void InputMaster::EjectButtonPressed(int playerId)
{
    if (MC->GetGameState() == GS_DEAD) {
        MC->SetGameState(GS_LOBBY);
        return;
//    } else if (MC->GetGameState() == GS_LOBBY) {
//        MC->GetPlayer(playerId)->EnterLobby();
    } else if (MC->GetGameState() != GS_PLAY || MC->IsPaused())
        return;

    //Keyboard
    if (playerId == 0) {
        for (Ship* s : MC->GetComponentsInScene<Ship>()) {

            if (s->IsEnabled()) {
                s->Eject();
                return;
            }
        }
    }
}

Vector3 InputMaster::GetMoveFromActions(Vector<PlayerInputAction>* actions)
{
    return Vector3{Vector3::RIGHT *
                (actions->Contains(PlayerInputAction::MOVE_RIGHT) -
                 actions->Contains(PlayerInputAction::MOVE_LEFT))

                + Vector3::FORWARD *
                (actions->Contains(PlayerInputAction::MOVE_UP) -
                 actions->Contains(PlayerInputAction::MOVE_DOWN))};
}
Vector3 InputMaster::GetAimFromActions(Vector<PlayerInputAction>* actions)
{
    return Vector3{ Vector3::RIGHT *
                (actions->Contains(PlayerInputAction::FIRE_E) -
                 actions->Contains(PlayerInputAction::FIRE_W))

                +   Vector3::FORWARD *
                (actions->Contains(PlayerInputAction::FIRE_N) -
                 actions->Contains(PlayerInputAction::FIRE_S))
                + Quaternion(45.0f, Vector3::UP) *
                   (Vector3::RIGHT *
                (actions->Contains(PlayerInputAction::FIRE_SE) -
                 actions->Contains(PlayerInputAction::FIRE_NW))

                +   Vector3::FORWARD *
                (actions->Contains(PlayerInputAction::FIRE_NE) -
                 actions->Contains(PlayerInputAction::FIRE_SW)))};
}

void InputMaster::Screenshot()
{
    Graphics* graphics{GetSubsystem<Graphics>()};
    Image screenshot{context_};
    graphics->TakeScreenShot(screenshot);
    //Here we save in the Data folder with date and time appended
    String fileName{GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
            Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png"};
    Log::Write(1, fileName);
    screenshot.SavePNG(fileName);
}
