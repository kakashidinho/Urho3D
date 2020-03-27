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

#ifndef HEXOCAM_H
#define HEXOCAM_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

namespace Urho3D {
class Node;
class Viewport;
class RenderPath;
class Camera;
}

using namespace Urho3D;

class heXoCam : public LogicComponent
{
    URHO3D_OBJECT(heXoCam, LogicComponent);
    friend class MasterControl;
    friend class InputMaster;

    void EnterLobby(StringHash eventType, VariantMap &eventData);
    void EnterPlay(StringHash eventType, VariantMap &eventData);
public:
    heXoCam(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;

    SharedPtr<Camera> camera_;
    Pair<SharedPtr<Camera>, SharedPtr<Camera> > stereoCam_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath_;

    void SetGreyScale(const bool enabled);

private:
    bool closeUp_;
    void SetupViewport();
};

#endif // HEXOCAM_H
